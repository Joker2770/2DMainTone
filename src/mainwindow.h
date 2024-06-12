#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QColor pixmapMainColor(const QImage &p, double bright = 1.0, unsigned int step = 20);

private slots:
    void onOpenActionTriggered();
    void onSaveActionTriggered();
    void onResultDisplay();
    void onRowSPBVChanged(int);
    void onColumnSPBVChanged(int);
    void onStepValueChanged(int);
    void onBrightnessValueChanged(double);

private:
    Ui::MainWindow *ui;

    QMenu *m_menu_file;
    QAction *m_action_open;
    QAction *m_action_save;

    QImage *m_origin_image;
    QImage *m_result_image;

    QGraphicsScene *m_origin_scene;
    QGraphicsScene *m_result_scene;

    QString m_origin_path;

    double m_brightness;
    unsigned int m_step;
};
#endif // MAINWINDOW_H
