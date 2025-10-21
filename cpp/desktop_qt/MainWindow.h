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

QT_BEGIN_NAMESPACE
class QAction;
class QTimer;
QT_END_NAMESPACE

namespace traductor {
    class TranslatorEngine;
    class Glossary;
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
    QTabWidget* tabWidget_;
    QTextEdit* textInput_;
    QTextEdit* textOutput_;
    QTextEdit* htmlInput_;
    QTextEdit* htmlOutput_;
    QComboBox* directionCombo_;
    QCheckBox* formalCheckBox_;
    QSpinBox* maxTokensSpinBox_;
    QPushButton* translateButton_;
    QPushButton* clearButton_;
    QPushButton* translateHtmlButton_;
    QPushButton* clearHtmlButton_;
    QPushButton* loadGlossaryButton_;
    
    // Status components
    QLabel* modelStatusLabel_;
    QLabel* cacheStatsLabel_;
    QLabel* latencyLabel_;
    QProgressBar* progressBar_;
    
    // State
    traductor::Glossary::TermMap glossary_;
    QTimer* metricsTimer_;
};

// MOC will be handled automatically by CMake
