#include "Installer.hpp"
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dirdlg.h>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

// ── Events ────────────────────────────────────────────────────────────────────
wxDEFINE_EVENT(EVT_INSTALL_LOG,  wxCommandEvent);
wxDEFINE_EVENT(EVT_INSTALL_PROG, wxCommandEvent);
wxDEFINE_EVENT(EVT_INSTALL_DONE, wxCommandEvent);

// ── Helpers ───────────────────────────────────────────────────────────────────

static wxFont monoFont(int pt = 9)
{
    return wxFont(pt, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
}

static wxColour bg()    { return wxColour(30, 30, 30); }
static wxColour panel() { return wxColour(37, 37, 38); }
static wxColour accent(){ return wxColour(0, 122, 204); }
static wxColour text()  { return wxColour(204, 204, 204); }
static wxColour dim()   { return wxColour(133, 133, 133); }

// ── Worker thread ─────────────────────────────────────────────────────────────

InstallWorker::InstallWorker(wxEvtHandler* sink, const std::string& installDir)
    : wxThread(wxTHREAD_DETACHED), sink_(sink), installDir_(installDir) {}

void InstallWorker::log(const std::string& msg)
{
    wxCommandEvent e(EVT_INSTALL_LOG);
    e.SetString(wxString::FromUTF8(msg));
    wxQueueEvent(sink_, e.Clone());
}

void InstallWorker::progress(int pct)
{
    wxCommandEvent e(EVT_INSTALL_PROG);
    e.SetInt(pct);
    wxQueueEvent(sink_, e.Clone());
}

bool InstallWorker::run(const std::string& label, const std::string& cmd)
{
    log("› " + label + "...");
    std::string full = "bash -c '" + cmd + "' 2>&1";
    FILE* pipe = popen(full.c_str(), "r");
    if (!pipe) { log("  ✗ Falha ao executar"); return false; }

    char buf[512];
    while (fgets(buf, sizeof(buf), pipe)) {
        std::string line = buf;
        if (!line.empty() && line.back() == '\n') line.pop_back();
        log("  " + line);
    }
    int rc = pclose(pipe);
    if (rc != 0) { log("  ✗ Erro (código " + std::to_string(rc) + ")"); return false; }
    log("  ✓ OK");
    return true;
}

wxThread::ExitCode InstallWorker::Entry()
{
    bool ok = true;
    progress(0);

    // Step 1 — system deps
    log("\n[1/5] Dependências do sistema");
    ok = run("apt-get install",
             "DEBIAN_FRONTEND=noninteractive apt-get install -y -qq "
             "build-essential g++ libsqlite3-dev git curl") && ok;
    progress(20);
    if (TestDestroy()) goto done;

    // Step 2 — nvm + Node 20
    log("\n[2/5] nvm + Node.js 20 LTS");
    ok = run("instalar nvm",
             "[ -d $HOME/.nvm ] || curl -fsSL "
             "https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash") && ok;
    ok = run("node 20",
             "export NVM_DIR=\"$HOME/.nvm\"; source \"$NVM_DIR/nvm.sh\"; "
             "nvm install 20 && nvm use 20 && nvm alias default 20") && ok;
    progress(40);
    if (TestDestroy()) goto done;

    // Step 3 — npm install
    log("\n[3/5] npm install");
    ok = run("npm install", "cd \"" + installDir_ + "\" && npm install --prefer-offline") && ok;
    progress(60);
    if (TestDestroy()) goto done;

    // Step 4 — compile C++
    log("\n[4/5] Compilar binário C++ (scaffold_zeus)");
    ok = run("g++ -lsqlite3",
             "cd \"" + installDir_ + "\" && "
             "export NVM_DIR=\"$HOME/.nvm\"; source \"$NVM_DIR/nvm.sh\"; "
             "npm run cpp:compile") && ok;
    progress(80);
    if (TestDestroy()) goto done;

    // Step 5 — verify SQLite DB
    log("\n[5/5] Verificar banco de dados SQLite");
    {
        std::string dbDir = std::string(getenv("HOME") ? getenv("HOME") : "/root") + "/.auxilius";
        fs::create_directories(dbDir);
        std::string bin = installDir_ + "/bin/scaffold_zeus";
        ok = run("scaffold_zeus db set installedAt",
                 "\"" + bin + "\" db set installedAt \"$(date -Iseconds)\"") && ok;
    }
    progress(100);

done:
    wxCommandEvent ev(EVT_INSTALL_DONE);
    ev.SetInt(ok ? 1 : 0);
    wxQueueEvent(sink_, ev.Clone());
    return nullptr;
}

// ── WelcomePage ───────────────────────────────────────────────────────────────

WelcomePage::WelcomePage(wxWizard* p) : wxWizardPageSimple(p)
{
    SetBackgroundColour(bg());

    auto* sz = new wxBoxSizer(wxVERTICAL);

    // Logo area
    auto* logoBox = new wxPanel(this);
    logoBox->SetBackgroundColour(accent());
    auto* logoSz = new wxBoxSizer(wxVERTICAL);
    auto* logo = new wxStaticText(logoBox, wxID_ANY, "{}");
    logo->SetFont(wxFont(40, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    logo->SetForegroundColour(*wxWHITE);
    logoSz->Add(logo, 0, wxALL | wxALIGN_CENTER, 24);
    logoBox->SetSizer(logoSz);
    sz->Add(logoBox, 0, wxEXPAND);

    sz->AddSpacer(24);

    auto* title = new wxStaticText(this, wxID_ANY, "Auxilius Installer");
    title->SetFont(wxFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title->SetForegroundColour(text());
    sz->Add(title, 0, wxLEFT | wxRIGHT, 32);

    sz->AddSpacer(8);

    auto* sub = new wxStaticText(this, wxID_ANY, "Zeus Retail Evolution — Developer Toolbox");
    sub->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    sub->SetForegroundColour(dim());
    sz->Add(sub, 0, wxLEFT | wxRIGHT, 32);

    sz->AddSpacer(24);
    sz->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 32);
    sz->AddSpacer(16);

    const wxString items[] = {
        "  ✓  C++20 scaffold engine com SQLite",
        "  ✓  Laravel DDD + React + Giro DS",
        "  ✓  Interface VS Code (Activity Bar, File Tree)",
        "  ✓  Sem Docker — instalação nativa",
    };
    for (const auto& item : items) {
        auto* t = new wxStaticText(this, wxID_ANY, item);
        t->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        t->SetForegroundColour(text());
        sz->Add(t, 0, wxLEFT | wxTOP, 32);
    }

    sz->AddStretchSpacer();
    auto* note = new wxStaticText(this, wxID_ANY, "Clique em Próximo para continuar.");
    note->SetForegroundColour(dim());
    note->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
    sz->Add(note, 0, wxALL, 32);

    SetSizer(sz);
}

// ── ConfigPage ────────────────────────────────────────────────────────────────

ConfigPage::ConfigPage(wxWizard* p) : wxWizardPageSimple(p)
{
    SetBackgroundColour(bg());
    auto* sz = new wxBoxSizer(wxVERTICAL);

    auto* title = new wxStaticText(this, wxID_ANY, "Configuração");
    title->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title->SetForegroundColour(text());
    sz->Add(title, 0, wxALL, 24);

    sz->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 24);
    sz->AddSpacer(20);

    auto* label = new wxStaticText(this, wxID_ANY, "Diretório de instalação:");
    label->SetForegroundColour(text());
    label->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_SEMIBOLD));
    sz->Add(label, 0, wxLEFT | wxRIGHT, 24);
    sz->AddSpacer(6);

    // Auto-detect: use the directory where this binary lives
    std::string defaultDir;
    {
        char buf[4096] = {};
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len > 0) {
            defaultDir = fs::path(buf).parent_path().parent_path().string();
        }
        if (defaultDir.empty()) defaultDir = std::string(getenv("HOME") ? getenv("HOME") : "/root") + "/auxilius";
    }

    auto* row = new wxBoxSizer(wxHORIZONTAL);
    dirCtrl_ = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(defaultDir));
    dirCtrl_->SetFont(monoFont(10));
    dirCtrl_->SetBackgroundColour(panel());
    dirCtrl_->SetForegroundColour(text());
    row->Add(dirCtrl_, 1, wxEXPAND | wxRIGHT, 8);

    auto* btn = new wxButton(this, wxID_ANY, "Navegar...");
    btn->SetBackgroundColour(panel());
    btn->SetForegroundColour(text());
    row->Add(btn, 0);
    sz->Add(row, 0, wxEXPAND | wxLEFT | wxRIGHT, 24);

    btn->Bind(wxEVT_BUTTON, &ConfigPage::OnBrowse, this);

    sz->AddSpacer(24);
    auto* note = new wxStaticText(this, wxID_ANY,
        "O instalador irá:\n"
        "  1. Instalar g++, libsqlite3-dev, git (apt)\n"
        "  2. Instalar nvm + Node.js 20 LTS\n"
        "  3. Executar npm install\n"
        "  4. Compilar scaffold_zeus com -lsqlite3\n"
        "  5. Inicializar ~/.auxilius/auxilius.db");
    note->SetForegroundColour(dim());
    note->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    sz->Add(note, 0, wxLEFT | wxRIGHT, 24);

    sz->AddStretchSpacer();
    SetSizer(sz);
}

std::string ConfigPage::getInstallDir() const
{
    return dirCtrl_->GetValue().ToStdString();
}

void ConfigPage::OnBrowse(wxCommandEvent&)
{
    wxDirDialog dlg(this, "Selecionar diretório", dirCtrl_->GetValue(),
                    wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK)
        dirCtrl_->SetValue(dlg.GetPath());
}

// ── ProgressPage ──────────────────────────────────────────────────────────────

ProgressPage::ProgressPage(wxWizard* p) : wxWizardPageSimple(p)
{
    SetBackgroundColour(bg());
    auto* sz = new wxBoxSizer(wxVERTICAL);

    auto* title = new wxStaticText(this, wxID_ANY, "Instalando...");
    title->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title->SetForegroundColour(text());
    sz->Add(title, 0, wxALL, 24);

    sz->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 24);
    sz->AddSpacer(12);

    gauge_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 6));
    gauge_->SetBackgroundColour(panel());
    sz->Add(gauge_, 0, wxEXPAND | wxLEFT | wxRIGHT, 24);
    sz->AddSpacer(8);

    status_ = new wxStaticText(this, wxID_ANY, "Aguardando...");
    status_->SetForegroundColour(dim());
    status_->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    sz->Add(status_, 0, wxLEFT | wxRIGHT, 24);
    sz->AddSpacer(8);

    log_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                          wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxHSCROLL);
    log_->SetFont(monoFont(9));
    log_->SetBackgroundColour(wxColour(20, 20, 20));
    log_->SetForegroundColour(text());
    sz->Add(log_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 24);

    SetSizer(sz);

    Bind(EVT_INSTALL_LOG,  &ProgressPage::OnLog,      this);
    Bind(EVT_INSTALL_PROG, &ProgressPage::OnProgress, this);
    Bind(EVT_INSTALL_DONE, &ProgressPage::OnDone,     this);
}

void ProgressPage::startInstall(const std::string& dir)
{
    done_ = false;
    log_->Clear();
    gauge_->SetValue(0);
    status_->SetLabel("Instalando...");

    auto* worker = new InstallWorker(this, dir);
    worker->Run();
}

void ProgressPage::OnLog(wxCommandEvent& e)
{
    log_->AppendText(e.GetString() + "\n");
}

void ProgressPage::OnProgress(wxCommandEvent& e)
{
    gauge_->SetValue(e.GetInt());
    status_->SetLabel(wxString::Format("Progresso: %d%%", e.GetInt()));
}

void ProgressPage::OnDone(wxCommandEvent& e)
{
    done_ = true;
    if (e.GetInt() == 1) {
        status_->SetLabel("✓ Instalação concluída com sucesso!");
        status_->SetForegroundColour(wxColour(78, 201, 176));
        gauge_->SetValue(100);
        // Auto-advance to done page
        wxWizardEvent adv(wxEVT_WIZARD_PAGE_CHANGING, GetId(), true);
        GetParent()->GetEventHandler()->ProcessEvent(adv);
    } else {
        status_->SetLabel("✗ Instalação falhou. Veja o log acima.");
        status_->SetForegroundColour(wxColour(244, 135, 113));
    }
    // Trigger wizard to re-evaluate button state
    wxWizardEvent fwd(wxEVT_WIZARD_PAGE_CHANGING, GetId(), true, this);
    GetParent()->GetEventHandler()->AddPendingEvent(fwd);
}

// ── DonePage ──────────────────────────────────────────────────────────────────

DonePage::DonePage(wxWizard* p) : wxWizardPageSimple(p)
{
    SetBackgroundColour(bg());
    auto* sz = new wxBoxSizer(wxVERTICAL);
    sz->AddStretchSpacer();

    auto* check = new wxStaticText(this, wxID_ANY, "✓");
    check->SetFont(wxFont(64, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    check->SetForegroundColour(wxColour(78, 201, 176));
    sz->Add(check, 0, wxALIGN_CENTER);

    sz->AddSpacer(16);

    auto* title = new wxStaticText(this, wxID_ANY, "Instalação concluída!");
    title->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title->SetForegroundColour(text());
    sz->Add(title, 0, wxALIGN_CENTER);

    sz->AddSpacer(12);

    auto* sub = new wxStaticText(this, wxID_ANY,
        "Para iniciar o Auxilius:\n\n  npm run dev");
    sub->SetFont(monoFont(11));
    sub->SetForegroundColour(dim());
    sub->SetWindowStyle(wxALIGN_CENTRE_HORIZONTAL);
    sz->Add(sub, 0, wxALIGN_CENTER);

    sz->AddStretchSpacer();

    auto* note = new wxStaticText(this, wxID_ANY,
        "Banco de dados: ~/.auxilius/auxilius.db\n"
        "Binário: bin/scaffold_zeus");
    note->SetFont(monoFont(9));
    note->SetForegroundColour(dim());
    sz->Add(note, 0, wxALL | wxALIGN_CENTER, 24);

    SetSizer(sz);
}

// ── InstallerWizard ───────────────────────────────────────────────────────────

wxBEGIN_EVENT_TABLE(InstallerWizard, wxWizard)
    EVT_WIZARD_PAGE_CHANGED(wxID_ANY,  InstallerWizard::OnPageChanged)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, InstallerWizard::OnPageChanging)
wxEND_EVENT_TABLE()

InstallerWizard::InstallerWizard()
    : wxWizard(nullptr, wxID_ANY, "Auxilius Installer",
               wxNullBitmap, wxDefaultPosition,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    SetBackgroundColour(bg());
    SetSize(wxSize(640, 520));

    welcome_  = new WelcomePage(this);
    config_   = new ConfigPage(this);
    progress_ = new ProgressPage(this);
    done_     = new DonePage(this);

    wxWizardPageSimple::Chain(welcome_,  config_);
    wxWizardPageSimple::Chain(config_,   progress_);
    wxWizardPageSimple::Chain(progress_, done_);

    GetPageAreaSizer()->Add(welcome_);
}

void InstallerWizard::OnPageChanging(wxWizardEvent& e)
{
    // Block advancing from progress page until done
    if (e.GetPage() == progress_ && e.GetDirection() && !progress_->AllowNext())
        e.Veto();
}

void InstallerWizard::OnPageChanged(wxWizardEvent& e)
{
    if (e.GetPage() == progress_)
        progress_->startInstall(config_->getInstallDir());
}

// ── InstallerApp ──────────────────────────────────────────────────────────────

bool InstallerApp::OnInit()
{
    if (!wxApp::OnInit()) return false;

    auto* wiz = new InstallerWizard();
    wiz->RunWizard(wiz->welcome_);
    wiz->Destroy();
    return false;  // exit after wizard closes
}

wxIMPLEMENT_APP(InstallerApp);
