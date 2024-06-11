#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDataStream>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->m_origin_image = new QImage();
    this->m_result_image = new QImage();

    this->m_origin_scene = new QGraphicsScene();
    this->m_result_scene = new QGraphicsScene();

    this->m_menu_file = new QMenu(this);
    this->m_menu_file->setTitle(tr("file"));

    this->m_action_open = new QAction(this);
    this->m_action_open->setText(tr("open"));
    this->m_action_open->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

    this->m_action_save = new QAction(this);
    this->m_action_save->setText(tr("save"));
    this->m_action_save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

    this->m_menu_file->addAction(this->m_action_open);
    this->m_menu_file->addAction(this->m_action_save);

    this->ui->graphicsView->setScene(this->m_origin_scene);
    this->ui->graphicsView_2->setScene(this->m_result_scene);

    ui->menubar->addMenu(this->m_menu_file);

    connect(this->m_action_open, SIGNAL(triggered()), this, SLOT(onOpenActionTriggered()));
    connect(this->m_action_save, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));
}

MainWindow::~MainWindow()
{
    if (nullptr != this->m_origin_scene)
    {
        delete this->m_origin_scene;
        this->m_origin_scene = nullptr;
    }
    if (nullptr != this->m_result_scene)
    {
        delete this->m_result_scene;
        this->m_result_scene = nullptr;
    }
    if (nullptr != this->m_origin_image)
    {
        delete this->m_origin_image;
        this->m_origin_image = nullptr;
    }
    if (nullptr != this->m_result_image)
    {
        delete this->m_result_image;
        this->m_result_image = nullptr;
    }
    if (nullptr != this->m_action_open)
    {
        delete this->m_action_open;
        this->m_action_open = nullptr;
    }
    if (nullptr != this->m_action_save)
    {
        delete this->m_action_save;
        this->m_action_save = nullptr;
    }
    if (nullptr != this->m_menu_file)
    {
        delete this->m_menu_file;
        this->m_menu_file = nullptr;
    }

    delete ui;
}

QColor MainWindow::pixmapMainColor(const QImage& image, double bright, unsigned int step)
{
    unsigned int u_step = step;
    if (u_step > 20 || u_step < 1)
        u_step = 1;
    int t=1;
    int r=0,g=0,b=0;
    for (int i=0;i<image.width();i+=u_step) {
        for (int j=0;j<image.height();j+=u_step) {
            if(image.valid(i,j)){
                t++;
                QColor c=image.pixel(i,j);
                r+=c.red();
                b+=c.blue();
                g+=c.green();
            }
        }
    }
    return QColor(int(bright*r/t)>255?255:int(bright*r/t),
                  int(bright*g/t)>255?255:int(bright*g/t),
                  int(bright*b/t)>255?255:int(bright*b/t));
}

void MainWindow::onOpenActionTriggered()
{
    QFileDialog qfd;
    qfd.setWindowModality(Qt::NonModal);
    QString path = qfd.getOpenFileName(this, tr("Open File"), ".", tr("Picture Files") + "Image Files(*.jpg *.jpeg *.png *.bmp)");
    if (!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, tr("Read File"), (tr("Cannot open file:") + "\n%1").arg(path));
            return;
        }
        file.close();

        this->m_origin_image->load(path);
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not input any text."));
        return;
    }

    this->m_origin_scene->clear();
    this->m_origin_image->scaled(this->ui->graphicsView->width(), this->ui->graphicsView->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->m_origin_scene->addPixmap(QPixmap::fromImage(*(this->m_origin_image)));
    this->ui->graphicsView->show();

    this->m_result_scene->clear();
    this->m_result_image->load(path);
    this->m_result_image->fill(QColor(0, 0, 0, 0));
    this->m_result_image->fill(pixmapMainColor(*(this->m_origin_image)));
    this->m_result_image->scaled(this->ui->graphicsView_2->width(), this->ui->graphicsView_2->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->m_result_scene->addPixmap(QPixmap::fromImage(*(this->m_result_image)));
    this->ui->graphicsView_2->show();
}

void MainWindow::onSaveActionTriggered()
{
    QFileDialog qfd;
    qfd.setWindowModality(Qt::NonModal);
    QString path = qfd.getSaveFileName(this, tr("Save File"), ".", tr("Picture Files") + "Image Files(*.jpg *.jpeg *.png *.bmp)");
    if (!path.isEmpty())
    {
        if (!(this->m_result_image->isNull()))
            this->m_result_image->save(path);
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not input any text."));
    }
}
