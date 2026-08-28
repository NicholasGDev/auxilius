#pragma once
#include <wx/wx.h>
#include <wx/wizard.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/thread.h>
#include <wx/dirctrl.h>
#include <functional>
#include <string>
#include <vector>

// ── Step IDs ──────────────────────────────────────────────────────────────────
enum PageId { PAGE_WELCOME = 0, PAGE_CONFIG, PAGE_PROGRESS, PAGE_DONE };

// ── Install step descriptor ───────────────────────────────────────────────────
struct Step {
    std::string label;
    std::string cmd;   // shell command to run
};

// ── Events sent from worker thread to UI ─────────────────────────────────────
wxDECLARE_EVENT(EVT_INSTALL_LOG,    wxCommandEvent);
wxDECLARE_EVENT(EVT_INSTALL_PROG,   wxCommandEvent);  // int = % 0-100
wxDECLARE_EVENT(EVT_INSTALL_DONE,   wxCommandEvent);  // int = success(1)/fail(0)

// ── Worker thread ─────────────────────────────────────────────────────────────
class InstallWorker : public wxThread
{
public:
    InstallWorker(wxEvtHandler* sink, const std::string& installDir);
    ExitCode Entry() override;

private:
    wxEvtHandler* sink_;
    std::string   installDir_;

    void log(const std::string& msg);
    void progress(int pct);
    bool run(const std::string& label, const std::string& cmd);
};

// ── Pages ─────────────────────────────────────────────────────────────────────

class WelcomePage : public wxWizardPageSimple
{
public:
    explicit WelcomePage(wxWizard* parent);
};

class ConfigPage : public wxWizardPageSimple
{
public:
    explicit ConfigPage(wxWizard* parent);
    std::string getInstallDir() const;

private:
    wxTextCtrl* dirCtrl_;
    void OnBrowse(wxCommandEvent&);
};

class ProgressPage : public wxWizardPageSimple
{
public:
    explicit ProgressPage(wxWizard* parent);
    void startInstall(const std::string& dir);

    void OnLog(wxCommandEvent& e);
    void OnProgress(wxCommandEvent& e);
    void OnDone(wxCommandEvent& e);

    bool AllowNext() const { return done_; }

private:
    wxTextCtrl* log_;
    wxGauge*    gauge_;
    wxStaticText* status_;
    bool done_ = false;
};

class DonePage : public wxWizardPageSimple
{
public:
    explicit DonePage(wxWizard* parent);
};

// ── Main wizard ───────────────────────────────────────────────────────────────

class InstallerWizard : public wxWizard
{
public:
    InstallerWizard();

public:
    WelcomePage*  welcome_;
private:
    ConfigPage*   config_;
    ProgressPage* progress_;
    DonePage*     done_;

    void OnPageChanged(wxWizardEvent& e);
    void OnPageChanging(wxWizardEvent& e);
    wxDECLARE_EVENT_TABLE();
};

// ── App ───────────────────────────────────────────────────────────────────────
class InstallerApp : public wxApp
{
public:
    bool OnInit() override;
};
