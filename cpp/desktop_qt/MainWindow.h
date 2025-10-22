#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QProgressBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

// Necesario porque usamos traductor::Glossary::TermMap en miembros
#include "../core/Glossary.h"
#include "../core/TranslatorEngine.h"

namespace traductor {
    // TranslatorEngine ya incluido arriba
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(traductor::TranslatorEngine& translator, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void translateText();
    void translateHtml();
    void clearText();
    void loadGlossary();
    void updateMetrics();
    void updateModelStatus();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void connectSignals();
    void updateCacheStats();

    traductor::TranslatorEngine& translator_;
    
    // UI Components
    QTabWidget* tabWidget_ = nullptr;
    QTextEdit* textInput_ = nullptr;
    QTextEdit* textOutput_ = nullptr;
    QTextEdit* htmlInput_ = nullptr;
    QTextEdit* htmlOutput_ = nullptr;
    QComboBox* directionCombo_ = nullptr;
    QCheckBox* formalCheckBox_ = nullptr;
    QSpinBox* maxTokensSpinBox_ = nullptr;
    QPushButton* translateButton_ = nullptr;
    QPushButton* clearButton_ = nullptr;
    QPushButton* translateHtmlButton_ = nullptr;
    QPushButton* clearHtmlButton_ = nullptr;
    QPushButton* loadGlossaryButton_ = nullptr;
    
    // Status components
    QLabel* modelStatusLabel_ = nullptr;
    QLabel* cacheStatsLabel_ = nullptr;
    QLabel* latencyLabel_ = nullptr;
    QProgressBar* progressBar_ = nullptr;
    
    // State
    traductor::Glossary::TermMap glossary_;
    QTimer* metricsTimer_ = nullptr;
};
