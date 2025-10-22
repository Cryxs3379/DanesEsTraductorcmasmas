#include "MainWindow.h"
#include "../core/Glossary.h"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QProgressBar>
#include <QFile>
#include <QTextStream>
#include <stdexcept>

MainWindow::MainWindow(traductor::TranslatorEngine& translator, QWidget* parent)
    : QMainWindow(parent), translator_(translator) {
    setupUI();
    setupMenuBar();
    setupStatusBar();
    connectSignals();
    
    // Update metrics every 2 seconds
    metricsTimer_ = new QTimer(this);
    connect(metricsTimer_, &QTimer::timeout, this, &MainWindow::updateMetrics);
    metricsTimer_->start(2000);
    
    updateModelStatus();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("Traductor Danés-Español");
    setMinimumSize(800, 600);
    resize(1200, 800);
    
    // Central widget with tabs
    tabWidget_ = new QTabWidget(this);
    setCentralWidget(tabWidget_);
    
    // Text translation tab
    QWidget* textTab = new QWidget;
    QVBoxLayout* textLayout = new QVBoxLayout(textTab);
    
    // Input/Output areas
    QHBoxLayout* textAreaLayout = new QHBoxLayout;
    
    QVBoxLayout* inputLayout = new QVBoxLayout;
    inputLayout->addWidget(new QLabel("Texto de entrada:"));
    textInput_ = new QTextEdit;
    textInput_->setPlaceholderText("Escriba el texto a traducir aquí...");
    inputLayout->addWidget(textInput_);
    
    QVBoxLayout* outputLayout = new QVBoxLayout;
    outputLayout->addWidget(new QLabel("Texto traducido:"));
    textOutput_ = new QTextEdit;
    textOutput_->setReadOnly(true);
    outputLayout->addWidget(textOutput_);
    
    textAreaLayout->addLayout(inputLayout);
    textAreaLayout->addLayout(outputLayout);
    textLayout->addLayout(textAreaLayout);
    
    // Control panel
    QHBoxLayout* controlLayout = new QHBoxLayout;
    
    controlLayout->addWidget(new QLabel("Dirección:"));
    directionCombo_ = new QComboBox;
    directionCombo_->addItem("Español → Danés", "es-da");
    directionCombo_->addItem("Danés → Español", "da-es");
    controlLayout->addWidget(directionCombo_);
    
    formalCheckBox_ = new QCheckBox("Formal (DA)");
    controlLayout->addWidget(formalCheckBox_);
    
    controlLayout->addWidget(new QLabel("Max tokens:"));
    maxTokensSpinBox_ = new QSpinBox;
    maxTokensSpinBox_->setRange(32, 8192);
    maxTokensSpinBox_->setValue(256);
    maxTokensSpinBox_->setSpecialValueText("Auto");
    maxTokensSpinBox_->setValue(0); // Auto by default
    controlLayout->addWidget(maxTokensSpinBox_);
    
    controlLayout->addStretch();
    
    translateButton_ = new QPushButton("Traducir");
    translateButton_->setDefault(true);
    controlLayout->addWidget(translateButton_);
    
    clearButton_ = new QPushButton("Limpiar");
    controlLayout->addWidget(clearButton_);
    
    textLayout->addLayout(controlLayout);
    tabWidget_->addTab(textTab, "Texto");
    
    // HTML translation tab
    QWidget* htmlTab = new QWidget;
    QVBoxLayout* htmlLayout = new QVBoxLayout(htmlTab);
    
    // HTML input/output
    QHBoxLayout* htmlAreaLayout = new QHBoxLayout;
    
    QVBoxLayout* htmlInputLayout = new QVBoxLayout;
    htmlInputLayout->addWidget(new QLabel("HTML de entrada:"));
    htmlInput_ = new QTextEdit;
    htmlInput_->setPlaceholderText("Pegue HTML de correo aquí...");
    htmlInputLayout->addWidget(htmlInput_);
    
    QVBoxLayout* htmlOutputLayout = new QVBoxLayout;
    htmlOutputLayout->addWidget(new QLabel("HTML traducido:"));
    htmlOutput_ = new QTextEdit;
    htmlOutput_->setReadOnly(true);
    htmlOutputLayout->addWidget(htmlOutput_);
    
    htmlAreaLayout->addLayout(htmlInputLayout);
    htmlAreaLayout->addLayout(htmlOutputLayout);
    htmlLayout->addLayout(htmlAreaLayout);
    
    // HTML controls (reuse same translation controls)
    QHBoxLayout* htmlControlLayout = new QHBoxLayout;
    translateHtmlButton_ = new QPushButton("Traducir HTML");
    clearHtmlButton_ = new QPushButton("Limpiar");
    htmlControlLayout->addWidget(translateHtmlButton_);
    htmlControlLayout->addWidget(clearHtmlButton_);
    htmlControlLayout->addStretch();
    htmlLayout->addLayout(htmlControlLayout);
    
    tabWidget_->addTab(htmlTab, "HTML");
    
    // Glossary controls
    QHBoxLayout* glossaryLayout = new QHBoxLayout;
    loadGlossaryButton_ = new QPushButton("Cargar Glosario");
    glossaryLayout->addWidget(loadGlossaryButton_);
    glossaryLayout->addStretch();
    textLayout->addLayout(glossaryLayout);
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = this->menuBar();
    
    QMenu* fileMenu = menuBar->addMenu("&Archivo");
    QAction* openAction = fileMenu->addAction("&Abrir...");
    QAction* saveAction = fileMenu->addAction("&Guardar traducción...");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("&Salir");
    
    QMenu* helpMenu = menuBar->addMenu("&Ayuda");
    QAction* aboutAction = helpMenu->addAction("&Acerca de...");
    
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "Acerca de", 
            "Traductor Danés-Español v1.0\n\n"
            "Traducción offline usando NLLB + CTranslate2\n"
            "100% privado - sin conexión a internet");
    });
}

void MainWindow::setupStatusBar() {
    modelStatusLabel_ = new QLabel("Modelo: Cargando...");
    cacheStatsLabel_ = new QLabel("Caché: 0 entradas");
    latencyLabel_ = new QLabel("Latencia: --ms");
    
    statusBar()->addWidget(modelStatusLabel_);
    statusBar()->addWidget(cacheStatsLabel_);
    statusBar()->addPermanentWidget(latencyLabel_);
    
    progressBar_ = new QProgressBar;
    progressBar_->setVisible(false);
    statusBar()->addPermanentWidget(progressBar_);
}

void MainWindow::connectSignals() {
    connect(translateButton_, &QPushButton::clicked, this, &MainWindow::translateText);
    connect(clearButton_, &QPushButton::clicked, this, &MainWindow::clearText);
    connect(translateHtmlButton_, &QPushButton::clicked, this, &MainWindow::translateHtml);
    connect(clearHtmlButton_, &QPushButton::clicked, this, &MainWindow::clearText);
    connect(loadGlossaryButton_, &QPushButton::clicked, this, &MainWindow::loadGlossary);
}

void MainWindow::translateText() {
    QString inputText = textInput_->toPlainText().trimmed();
    if (inputText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor, ingrese texto para traducir.");
        return;
    }
    
    progressBar_->setVisible(true);
    progressBar_->setRange(0, 0); // Indeterminate
    
    QString direction = directionCombo_->currentData().toString();
    bool formal = formalCheckBox_->isChecked();
    int maxTokens = maxTokensSpinBox_->value() > 0 ? maxTokensSpinBox_->value() : -1;
    
    try {
        std::string result = translator_.translate(
            inputText.toStdString(),
            direction.toStdString(),
            maxTokens,
            formal,
            glossary_
        );
        
        textOutput_->setPlainText(QString::fromStdString(result));
        
        updateCacheStats();
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error de Traducción", 
            QString("Error al traducir: %1").arg(e.what()));
    }
    
    progressBar_->setVisible(false);
}

void MainWindow::translateHtml() {
    // Similar to translateText but for HTML
    QString inputHtml = htmlInput_->toPlainText().trimmed();
    if (inputHtml.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor, ingrese HTML para traducir.");
        return;
    }
    
    progressBar_->setVisible(true);
    progressBar_->setRange(0, 0);
    
    QString direction = directionCombo_->currentData().toString();
    bool formal = formalCheckBox_->isChecked();
    int maxTokens = maxTokensSpinBox_->value() > 0 ? maxTokensSpinBox_->value() : -1;
    
    try {
        std::string result = translator_.translateHtml(
            inputHtml.toStdString(),
            direction.toStdString(),
            maxTokens,
            formal,
            glossary_
        );
        
        htmlOutput_->setPlainText(QString::fromStdString(result));
        updateCacheStats();
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error de Traducción HTML", 
            QString("Error al traducir HTML: %1").arg(e.what()));
    }
    
    progressBar_->setVisible(false);
}

void MainWindow::clearText() {
    if (tabWidget_->currentIndex() == 0) { // Text tab
        textInput_->clear();
        textOutput_->clear();
    } else { // HTML tab
        htmlInput_->clear();
        htmlOutput_->clear();
    }
}

void MainWindow::loadGlossary() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Cargar Glosario",
        "",
        "Archivos de texto (*.txt);;Todos los archivos (*)"
    );
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            
            traductor::Glossary glossary;
            if (glossary.loadFromString(content.toStdString())) {
                glossary_ = glossary.getTerms();
                QMessageBox::information(this, "Glosario", 
                    QString("Glosario cargado con %1 términos").arg(glossary_.size()));
            } else {
                QMessageBox::warning(this, "Error", "No se pudo cargar el glosario.");
            }
        }
    }
}

void MainWindow::updateMetrics() {
    updateModelStatus();
    updateCacheStats();
    
    double avgLatency = translator_.getAverageLatency();
    latencyLabel_->setText(QString("Latencia: %1ms").arg(avgLatency, 0, 'f', 1));
}

void MainWindow::updateModelStatus() {
    auto health = translator_.getHealthInfo();
    
    if (health.modelLoaded && health.tokenizerLoaded) {
        modelStatusLabel_->setText("Modelo: ✓ Listo");
        modelStatusLabel_->setStyleSheet("color: green;");
        modelStatusLabel_->setToolTip("Modelo cargado correctamente");
    } else {
        modelStatusLabel_->setText("Modelo: ✗ Error");
        modelStatusLabel_->setStyleSheet("color: red;");
        
        // Mostrar error detallado en tooltip
        QString errorMsg = QString("Error: %1\n\nRutas esperadas:\n- Tokenizer: ./models/nllb-600m/sentencepiece.bpe.model\n- Modelo: ./models/nllb-600m-ct2-int8/")
                          .arg(QString::fromStdString(health.lastError));
        modelStatusLabel_->setToolTip(errorMsg);
    }
}

void MainWindow::updateCacheStats() {
    auto health = translator_.getHealthInfo();
    cacheStatsLabel_->setText(
        QString("Caché: %1 entradas (%2% hit rate)")
        .arg(health.cacheSize)
        .arg(health.cacheHitRate, 0, 'f', 1)
    );
}
