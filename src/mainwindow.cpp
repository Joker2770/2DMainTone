#include "separator.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDataStream>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/res/icon.png"));

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

    this->m_origin_path.clear();

    this->m_brightness = 1.0;
    this->m_step = 20;

    connect(this->m_action_open, SIGNAL(triggered()), this, SLOT(onOpenActionTriggered()));
    connect(this->m_action_save, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));
    connect(this->ui->row_spb, SIGNAL(valueChanged(int)), this, SLOT(onRowSPBVChanged(int)));
    connect(this->ui->column_spb, SIGNAL(valueChanged(int)), this, SLOT(onColumnSPBVChanged(int)));
    connect(this->ui->brightness_dspb, SIGNAL(valueChanged(double)), this, SLOT(onBrightnessValueChanged(double)));
    connect(this->ui->step_spb, SIGNAL(valueChanged(int)), this, SLOT(onStepValueChanged(int)));
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

QColor MainWindow::pixmapMainColor(const QImage &image, double bright, unsigned int step)
{
    unsigned int u_step = step;
    if (u_step > 99 || u_step < 1)
        u_step = 1;
    if (u_step < image.width())
        u_step = image.width() - 1;
    if (u_step < image.height())
        u_step = image.height() - 1;
    int t = 1;
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < image.width(); i += u_step)
    {
        for (int j = 0; j < image.height(); j += u_step)
        {
            if (image.valid(i, j))
            {
                t++;
                QColor c = image.pixel(i, j);
                r += c.red();
                b += c.blue();
                g += c.green();
            }
        }
    }
    return QColor(int(bright * r / t) > 255 ? 255 : int(bright * r / t),
                  int(bright * g / t) > 255 ? 255 : int(bright * g / t),
                  int(bright * b / t) > 255 ? 255 : int(bright * b / t));
}

void MainWindow::onOpenActionTriggered()
{
    QFileDialog qfd;
    qfd.setWindowModality(Qt::NonModal);
    this->m_origin_path = qfd.getOpenFileName(this, tr("Open File"), ".", tr("Picture Files") + "Image Files(*.jpg *.jpeg *.png *.bmp)");
    if (!this->m_origin_path.isEmpty())
    {
        QFile file(this->m_origin_path);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, tr("Read File"), (tr("Cannot open file:") + "\n%1").arg(this->m_origin_path));
            return;
        }
        file.close();

        this->m_origin_image->load(this->m_origin_path);
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

    this->onResultDisplay();
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

void MainWindow::onResultDisplay()
{
    Separator s;
    this->m_result_scene->clear();
    this->m_result_image->load(this->m_origin_path);
    this->m_result_image->fill(QColor(0, 0, 0, 0));
    unsigned int row = (unsigned int)this->ui->row_spb->value();
    unsigned int column = (unsigned int)this->ui->column_spb->value();
    std::vector<std::pair<QPoint, QPoint>> coord_list = s.get_coordinate_separation_with_2D(this->m_result_image->width(),
                                                                                            this->m_result_image->height(),
                                                                                            row,
                                                                                            column);
    QPixmap pm_out = QPixmap::fromImage(*(this->m_result_image));
    QPainter painter(&pm_out);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    for (const auto &v : coord_list)
    {
        QPixmap pm_u(v.second.x() - v.first.x(), v.second.y() - v.first.y());
        QRect rect(v.first.x(), v.first.y(), v.second.x(), v.second.y());
        QImage cop_img = this->m_origin_image->copy(rect);
        pm_u.fill(QColor(this->pixmapMainColor(cop_img, this->m_brightness, this->m_step)));
        painter.drawPixmap(v.first.x(), v.first.y(), pm_u, 0, 0, v.second.x() - v.first.x(), v.second.y() - v.first.y());
    }
    *(this->m_result_image) = pm_out.toImage();
    this->m_result_image->scaled(this->ui->graphicsView_2->width(), this->ui->graphicsView_2->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->m_result_scene->addPixmap(QPixmap::fromImage(*(this->m_result_image)));
    this->ui->graphicsView_2->show();
}

void MainWindow::onRowSPBVChanged(int val)
{
    Q_UNUSED(val)
    this->onResultDisplay();
}

void MainWindow::onColumnSPBVChanged(int val)
{
    Q_UNUSED(val)
    this->onResultDisplay();
}

void MainWindow::onStepValueChanged(int val)
{
    this->m_step = val;
    this->onResultDisplay();
}

void MainWindow::onBrightnessValueChanged(double val)
{
    this->m_brightness = val;
    this->onResultDisplay();
}
