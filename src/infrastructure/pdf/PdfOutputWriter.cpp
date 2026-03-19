
#include "pch.h"
#include "framework.h"
#include "PdfOutputWriter.h"
#include "FileOutputTarget.h"
#include "SageException.h"
#include <vector>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// PDF 1.4 raw 직접 생성 (외부 라이브러리 없음)
// 폰트: MalgunGothic / Identity-H (한글·영문 모두 지원)
// 텍스트: UTF-16BE 헥스 인코딩 (<FEFF...>)
// ============================================================

namespace {

// ── PDF 레이아웃 상수 ─────────────────────────────────────
constexpr float PAGE_W    = 595.3f;   // A4 가로 (pt)
constexpr float PAGE_H    = 841.9f;   // A4 세로 (pt)
constexpr float MARGIN_X  = 36.0f;
constexpr float MARGIN_Y  = 40.0f;
constexpr float ROW_H     = 18.0f;
constexpr float FONT_SZ   = 9.0f;
constexpr float CELL_PAD  = 3.0f;
constexpr int   MAX_COLS  = 20;

// ── 문자 너비 추정 (FONT_SZ 기준, pt) ────────────────────
static float ChrW(wchar_t c)
{
    if ((c >= 0xAC00 && c <= 0xD7A3) ||
        (c >= 0x4E00 && c <= 0x9FFF) ||
        (c >= 0x3000 && c <= 0x303F))
        return FONT_SZ * 0.64f;
    return FONT_SZ * 0.54f;
}

static float EstW(const CString& s)
{
    float w = 0;
    LPCWSTR p = (LPCWSTR)(LPCTSTR)s;
    while (*p) w += ChrW(*p++);
    return w;
}

// ── 최대 너비에 맞게 자르기 ───────────────────────────────
static CString Trunc(const CString& s, float maxW)
{
    if (EstW(s) <= maxW) return s;
    const float dotW = ChrW(L'.') * 3;
    CString r;
    float cur = 0;
    LPCWSTR p = (LPCWSTR)(LPCTSTR)s;
    while (*p) {
        float cw = ChrW(*p);
        if (cur + cw + dotW > maxW) { r += _T("..."); break; }
        r += *p++;
        cur += cw;
    }
    return r;
}

// ── CString → PDF UTF-16BE 헥스 문자열 ───────────────────
static std::string HexStr(const CString& s)
{
    std::string r;
    r.reserve((size_t)s.GetLength() * 4 + 8);
    r = "<FEFF";
    LPCWSTR p = (LPCWSTR)(LPCTSTR)s;
    char buf[5];
    while (*p) {
        sprintf_s(buf, sizeof(buf), "%04X", (unsigned short)(*p++));
        r += buf;
    }
    r += ">";
    return r;
}

static std::string Ff(float v)
{
    char buf[32];
    sprintf_s(buf, sizeof(buf), "%.3f", v);
    return buf;
}

// ─────────────────────────────────────────────────────────────
// PdfWriter — PDF 파일 구조 관리
// ─────────────────────────────────────────────────────────────
class PdfWriter
{
public:
    PdfWriter(CFile& f, const TabularData& d) : m_file(f), m_data(d) {}
    void Generate();

private:
    CFile&             m_file;
    const TabularData& m_data;
    std::vector<long>  m_offsets;  // 객체 오프셋 (1-indexed → [id-1])

    void   Out(const char* sz)        { m_file.Write(sz, (UINT)strlen(sz)); }
    void   Out(const std::string& s)  { m_file.Write(s.c_str(), (UINT)s.size()); }
    long   Pos() const                { return (long)m_file.GetPosition(); }
    int    AllocId()                  { m_offsets.push_back(-1L); return (int)m_offsets.size(); }
    void   BeginObj(int id)           { m_offsets[id-1] = Pos(); char b[32]; sprintf_s(b, sizeof(b), "%d 0 obj\n", id); Out(b); }
    void   EndObj()                   { Out("endobj\n"); }

    std::vector<float> CalcColWidths(const DataSheet& sh);
    std::string        BuildContent(const DataSheet& sh,
                                    const std::vector<float>& cw,
                                    const std::vector<int>& rowIdx);
    void               WriteFontObjs(int idFont, int idCID);
    void               WriteXRef();
};

// ── 컬럼 너비 계산 ────────────────────────────────────────
std::vector<float> PdfWriter::CalcColWidths(const DataSheet& sh)
{
    int nCols = sh.GetColumnCount();
    if (nCols > MAX_COLS) nCols = MAX_COLS;
    if (nCols == 0) return {};

    std::vector<float> colW(nCols, 40.0f);

    int nSample = sh.GetRowCount();
    if (nSample > 50) nSample = 50;
    for (int r = 0; r < nSample; r++) {
        const std::vector<CString>& row = sh.m_arrRows[r];
        for (int c = 0; c < nCols && c < (int)row.size(); c++) {
            float w = EstW(row[c]) + CELL_PAD * 2;
            if (w > colW[c]) colW[c] = w;
        }
    }

    for (int c = 0; c < nCols; c++) {
        if (colW[c] > 160.0f) colW[c] = 160.0f;
        if (colW[c] < 40.0f)  colW[c] = 40.0f;
    }

    float avail = PAGE_W - MARGIN_X * 2;
    float total = 0;
    for (float w : colW) total += w;
    if (total > avail) {
        float scale = avail / total;
        for (float& w : colW) w *= scale;
    }
    return colW;
}

// ── 페이지 콘텐츠 스트림 생성 ─────────────────────────────
// rowIdx: 이 페이지에 렌더링할 sheet 행 인덱스 목록
// rowIdx[0] == 0 이면 헤더 행 (색상 구분)
std::string PdfWriter::BuildContent(const DataSheet& sh,
                                     const std::vector<float>& cw,
                                     const std::vector<int>& rowIdx)
{
    std::string s;
    s.reserve(32768);

    int   nCols = (int)cw.size();
    float tblX  = MARGIN_X;
    float tblW  = 0;
    for (float w : cw) tblW += w;
    float yTop  = PAGE_H - MARGIN_Y;

    char buf[512];

    // 1) 행 배경 사각형 ────────────────────────────────────
    for (int i = 0; i < (int)rowIdx.size(); i++) {
        float rBot  = yTop - (i + 1) * ROW_H;
        bool  isHdr = (rowIdx[i] == 0);
        bool  isOdd = (i % 2 == 1);

        if (isHdr)
            s += "0.24 0.27 0.35 rg\n";
        else if (isOdd)
            s += "0.95 0.95 0.97 rg\n";
        else
            s += "1 1 1 rg\n";

        sprintf_s(buf, sizeof(buf), "%.3f %.3f %.3f %.3f re f\n",
                  tblX, rBot, tblW, ROW_H);
        s += buf;
    }

    // 2) 셀 텍스트 ─────────────────────────────────────────
    s += "BT\n/F1 ";
    s += Ff(FONT_SZ);
    s += " Tf\n";

    for (int i = 0; i < (int)rowIdx.size(); i++) {
        int   ri    = rowIdx[i];
        float rBot  = yTop - (i + 1) * ROW_H;
        float txtY  = rBot + (ROW_H - FONT_SZ) * 0.45f;
        bool  isHdr = (ri == 0);

        if (isHdr)
            s += "1 1 1 rg\n";
        else
            s += "0.1 0.1 0.1 rg\n";

        const std::vector<CString>& row = sh.m_arrRows[ri];
        float cellX = tblX;
        for (int c = 0; c < nCols; c++) {
            CString cell;
            if (c < (int)row.size()) cell = row[c];
            cell = Trunc(cell, cw[c] - CELL_PAD * 2);

            sprintf_s(buf, sizeof(buf),
                      "1 0 0 1 %.3f %.3f Tm\n", cellX + CELL_PAD, txtY);
            s += buf;
            s += HexStr(cell);
            s += " Tj\n";
            cellX += cw[c];
        }
    }
    s += "ET\n";

    // 3) 테두리 선 ─────────────────────────────────────────
    int   nRows  = (int)rowIdx.size();
    float tblBot = yTop - nRows * ROW_H;

    for (int i = 0; i <= nRows; i++) {
        float y     = yTop - i * ROW_H;
        bool  thick = (i == 0 || i == nRows);
        if (thick) s += "0 0 0 RG\n0.6 w\n";
        else        s += "0.75 0.75 0.75 RG\n0.3 w\n";
        sprintf_s(buf, sizeof(buf),
                  "%.3f %.3f m %.3f %.3f l S\n", tblX, y, tblX + tblW, y);
        s += buf;
    }

    float lx = tblX;
    for (int c = 0; c <= nCols; c++) {
        bool thick = (c == 0 || c == nCols);
        if (thick) s += "0 0 0 RG\n0.6 w\n";
        else        s += "0.75 0.75 0.75 RG\n0.3 w\n";
        sprintf_s(buf, sizeof(buf),
                  "%.3f %.3f m %.3f %.3f l S\n", lx, yTop, lx, tblBot);
        s += buf;
        if (c < nCols) lx += cw[c];
    }

    return s;
}

// ── 폰트 객체 작성 (Type0 CID + Identity-H) ──────────────
void PdfWriter::WriteFontObjs(int idFont, int idCID)
{
    BeginObj(idFont);
    char buf[256];
    sprintf_s(buf, sizeof(buf),
        "<< /Type /Font /Subtype /Type0\n"
        "   /BaseFont /MalgunGothic\n"
        "   /Encoding /Identity-H\n"
        "   /DescendantFonts [%d 0 R]\n"
        ">>\n",
        idCID);
    Out(buf);
    EndObj();

    BeginObj(idCID);
    Out("<< /Type /Font /Subtype /CIDFontType2\n"
        "   /BaseFont /MalgunGothic\n"
        "   /CIDSystemInfo\n"
        "   << /Registry (Adobe) /Ordering (Identity) /Supplement 0 >>\n"
        "   /DW 1000\n"
        ">>\n");
    EndObj();
}

// ── XRef 테이블 + Trailer 작성 ────────────────────────────
void PdfWriter::WriteXRef()
{
    long xrefOff = Pos();
    int  nObjs   = (int)m_offsets.size();

    char buf[128];
    sprintf_s(buf, sizeof(buf), "xref\n0 %d\n", nObjs + 1);
    Out(buf);
    Out("0000000000 65535 f \n");

    for (int i = 0; i < nObjs; i++) {
        sprintf_s(buf, sizeof(buf), "%010ld 00000 n \n", m_offsets[i]);
        Out(buf);
    }

    sprintf_s(buf, sizeof(buf),
        "trailer\n<< /Size %d /Root 1 0 R >>\n"
        "startxref\n%ld\n%%%%EOF\n",
        nObjs + 1, xrefOff);
    Out(buf);
}

// ── 메인 생성 함수 ────────────────────────────────────────
void PdfWriter::Generate()
{
    const DataSheet& sh = m_data.GetSheet(0);

    std::vector<float> colW = CalcColWidths(sh);
    if (colW.empty())
        throw SageException(_T("PDF 생성 오류: 컬럼이 없습니다."));

    // 페이지당 최대 행 수
    int nMaxRows     = (int)((PAGE_H - MARGIN_Y * 2) / ROW_H);
    if (nMaxRows < 2) nMaxRows = 2;
    int nDataPerPage = nMaxRows - 1;   // 헤더 1행 제외

    // 페이지별 행 인덱스 목록 구성 (헤더=행 0 을 매 페이지 반복)
    int nTotalRows = sh.GetRowCount();
    int nDataRows  = nTotalRows > 1 ? nTotalRows - 1 : 0;

    std::vector<std::vector<int>> pageRows;
    if (nDataRows == 0) {
        pageRows.push_back({0});
    } else {
        for (int d = 0; d < nDataRows; d += nDataPerPage) {
            std::vector<int> pg;
            pg.push_back(0);  // 헤더 행
            int end = d + nDataPerPage;
            if (end > nDataRows) end = nDataRows;
            for (int r = d + 1; r <= end; r++)
                pg.push_back(r);
            pageRows.push_back(pg);
        }
    }
    int nPages = (int)pageRows.size();

    // 객체 ID 사전 할당
    int idCatalog = AllocId();   // 1
    int idPages   = AllocId();   // 2
    int idFont    = AllocId();   // 3
    int idCID     = AllocId();   // 4

    std::vector<int> pageObjIds;
    std::vector<int> contentIds;
    for (int p = 0; p < nPages; p++) {
        pageObjIds.push_back(AllocId());
        contentIds.push_back(AllocId());
    }

    // PDF 헤더
    Out("%PDF-1.4\n%\xE2\xE3\xCF\xD3\n");

    // Catalog
    BeginObj(idCatalog);
    char buf[512];
    sprintf_s(buf, sizeof(buf), "<< /Type /Catalog /Pages %d 0 R >>\n", idPages);
    Out(buf);
    EndObj();

    // Pages
    BeginObj(idPages);
    Out("<< /Type /Pages /Kids [");
    for (int p = 0; p < nPages; p++) {
        sprintf_s(buf, sizeof(buf), "%d 0 R ", pageObjIds[p]);
        Out(buf);
    }
    sprintf_s(buf, sizeof(buf), "] /Count %d >>\n", nPages);
    Out(buf);
    EndObj();

    // 폰트 객체
    WriteFontObjs(idFont, idCID);

    // 콘텐츠 스트림 + 페이지 객체
    for (int p = 0; p < nPages; p++) {
        std::string content = BuildContent(sh, colW, pageRows[p]);

        BeginObj(contentIds[p]);
        sprintf_s(buf, sizeof(buf), "<< /Length %d >>\nstream\n", (int)content.size());
        Out(buf);
        Out(content);
        Out("endstream\n");
        EndObj();

        BeginObj(pageObjIds[p]);
        sprintf_s(buf, sizeof(buf),
            "<< /Type /Page\n"
            "   /Parent %d 0 R\n"
            "   /MediaBox [0 0 %.3f %.3f]\n"
            "   /Resources << /Font << /F1 %d 0 R >> >>\n"
            "   /Contents %d 0 R\n"
            ">>\n",
            idPages, PAGE_W, PAGE_H, idFont, contentIds[p]);
        Out(buf);
        EndObj();
    }

    WriteXRef();
}

} // namespace

// ============================================================
void PdfOutputWriter::Write(const TabularData& data,
                             const IOutputTarget& target)
{
    const FileOutputTarget* pTarget =
        dynamic_cast<const FileOutputTarget*>(&target);
    if (pTarget == nullptr)
        throw SageException(_T("PdfOutputWriter: 잘못된 출력 대상입니다."));

    if (data.GetSheetCount() == 0 || data.GetSheet(0).GetRowCount() == 0)
        throw SageException(_T("내보낼 데이터가 없습니다."));

    const CString& strPath = pTarget->GetFilePath();

    CFile file;
    if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
        CString msg;
        msg.Format(_T("파일을 열 수 없습니다: %s"), (LPCTSTR)strPath);
        throw SageException(msg);
    }

    PdfWriter writer(file, data);
    writer.Generate();
    file.Close();
}
