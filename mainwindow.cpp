#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DDD.h"
#include "approx.h"
#include <cstdlib>
#include <ctime>

double f_1(double , double){
    return 1;
}
double f_2(double x, double){
    return x;
}
double f_3(double , double y){
    return y;
}
double f_4(double x, double y){
    return x + y;
}

double f_5(double x, double y){
    return x*y + x*x ;
}

double f_6(double x, double y){
    return exp(x)*exp(y);
}

double f_7(double x, double y){
    return x*y*(10 - x - y);
}

MainWindow::MainWindow(int bs_x , int bs_y ,QWidget *parent)
    :  QMainWindow(parent), base_nx(bs_x) , base_ny(bs_y),par(l1 , l2 , alpha, k , base_nx, base_ny),base_nx_rect(par.nx_rect) ,base_ny_rect(par.ny_rect) ,
   ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    nx = base_nx;
    ny = base_ny;


    functions.push_back({f_1, "f(x, y) = 1"});
    functions.push_back({f_2, "f(x, y) = x"});
    functions.push_back({f_3, "f(x, y) = y"});
    functions.push_back({f_4, "f(x, y) = x + y"});
    functions.push_back({f_5, "f(x, y) = x*y + x*x"});
    functions.push_back({f_6, "f(x, y) = e^x * e^y"});
    functions.push_back({f_7, "f(x, y) = x*y*(10 - x - y)"});


    ui->functionLabel->setText(functions[0].second);
    connect(ui->changeFunctionPushButton, &QPushButton::released, this, [&]() {
        setCurrentFunction((currentFunctionIndex + 1) % functions.size());
    });
    connect(ui->nmlt2_pb, &QPushButton::released, this, [&]() {par.new_par(true, base_nx , base_ny);
        nx = par.nx; ny = par.ny;
        ui->nxLineEdit->setText(QString::number(nx));
        ui->nyLineEdit->setText(QString::number(ny));
        setParallelogram();
    });

    connect(ui->ndiv2_pb, &QPushButton::released, this, [&]() {par.new_par(false, base_nx , base_ny);
        nx = par.nx; ny = par.ny;
        ui->nxLineEdit->setText(QString::number(nx));
        ui->nyLineEdit->setText(QString::number(ny));
        setParallelogram();
    });




    ui->l1LineEdit->setText(QString::number(l1));
    ui->l2LineEdit->setText(QString::number(l2));
    ui->alphaLineEdit->setText(QString::number(alpha));
    ui->kLineEdit->setText(QString::number(k));
    ui->nxLineEdit->setText(QString::number(nx));
    ui->nyLineEdit->setText(QString::number(ny));
    ui->pLineEdit->setText(QString::number(p));
    connect(ui->computePushButton, &QPushButton::released, this, &MainWindow::setParallelogram);

    srand(time(0));


    funcRange();
    ui->computePushButton->animateClick();

    computeTimer.setInterval(300);
    computeTimer.callOnTimeout(this, [&]() {
        if (allThreadsPokushali) {
            qDebug() << "spasipo, o4en' vkusno";
            freeThreadVars();
            activate();

            //            nx = 1 + rand() % 2048;
            //            ny = 1 + rand() % 2048;
            //            p = 1 + rand() % 7;
            //            l1 = 1 + rand() % 5;
            //            l2 = 1 + rand() % 5;
            //            alpha = 10 + rand() % 80;
            //            k = rand() % 90 / 100.0;
            //            LOG(nx);
            //            LOG(ny);
            //            LOG(p);
            //            LOG(l1);
            //            LOG(l2);
            //            LOG(alpha);
            //            LOG(k);
            //            LOG(currentFunctionIndex);
            //            ui->l1LineEdit->setText(QString::number(l1));
            //            ui->l2LineEdit->setText(QString::number(l2));
            //            ui->alphaLineEdit->setText(QString::number(alpha));
            //            ui->kLineEdit->setText(QString::number(k));
            //            ui->nxLineEdit->setText(QString::number(nx));
            //            ui->nyLineEdit->setText(QString::number(ny));
            //            ui->pLineEdit->setText(QString::number(p));
            //            if (rand() % 5 == 1) {
            //                ui->changeFunctionPushButton->animateClick();
            //            }
            //            compute();
        }
    });

    computeTimer.start();
}

void MainWindow::funcRange() {
    yRange.first = 1e9;
    yRange.second = -1e9;
    auto corner = par.cord_trans(par.xs[nx] , par.ys[ny], 0);
    xRange = {0, corner.x()};
    zRange = {0, corner.z()};
    const double step = (xRange.second - xRange.first) / 100;
    for (double x = xRange.first; x <= xRange.second; x += step) {
        for (double z = zRange.first; z <= zRange.second; z += step) {
            if (currentFunction()(x, z) > yRange.second) {
                yRange.second = currentFunction()(x, z);
            }
            if (currentFunction()(x, z) < yRange.first) {
                yRange.first = currentFunction()(x, z);
            }
        }
    }
    if (std::abs(yRange.first - yRange.second) < 1e-15) {
        yRange.first = yRange.first - 1;
        yRange.second = yRange.second + 1;
    }
}


double  MainWindow::f_aprox_value(double x , double y){
    int i = x/par.hx;
    int j = y/par.hy;
    double k1 = (x  - i*par.hx)/par.hx;
    double k2 = (y - j*par.hy) / par.hy;
    double res = 0;

    //double res = x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i, j )];
    if(i>=nx - par.nx_rect && j>=ny - par.ny_rect){
        i = nx - par.nx_rect - 1;
        j = ny - par.ny_rect - 1;
    }
    if(i>=nx - par.nx_rect && j>=ny - par.ny_rect){

    }
    if(k2 - k1 > 0){
         k2 = 1 - k2;
        // qDebug()<<get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i, j) << i << j;
        res = k1*x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i + 1, j + 1)]
         + k2*x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i, j )]
         + (1 - k1 - k2)*x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i, j + 1 )];
    }else{
        k1 = 1 - k1;
        res = k1*x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i , j )]
         + k2*x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i + 1, j + 1 )]
         + (1 - k1 - k2)*x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i + 1, j)];
    }
//    if(k1 < 0.5 && k2 < 0.5){ return x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i, j )];}
//    else if(k2>=0.5 && k1 < 0.5){ return x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i, j+1 )];}
//    else if(k1>=0.5 && k2 < 0.5){ return x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i + 1, j )];}
//    else {return x_copy[get_k(par.nx , par.ny , par.nx_rect , par.ny_rect , i + 1, j + 1 )];}

    return res;
}

vector<Triangle> MainWindow::func_apr_trio(const QVector3D& size ) {
    const QVector3D ex = {size.x(), 0, 0};
    const QVector3D ey = {0, size.y(), 0};
    const QVector3D ez = {0, 0, size.z()};

    const QVector3D a = -size / 2;
    const QVector3D b = a + ez;
    const QVector3D c = b + ex;
    const QVector3D d = c - ez;
    const QVector3D e = a + ey;

    const double xDiff = xRange.second - xRange.first;
    const double yDiff = yRange.second - yRange.first;
    const double zDiff = zRange.second - zRange.first;

    // переводит точку из системы координат графика в НСК
    auto toNdc = [&](const QVector3D& p) {
        float x = a.x() + (d.x() - a.x()) * ((p.x() - xRange.first) / xDiff);
        float y = a.y() + (e.y() - a.y()) * ((p.y() - yRange.first) / yDiff);
        float z = a.z() + (b.z() - a.z()) * ((p.z() - zRange.first) / zDiff);
        return QVector3D(x, y, z);
    };
    int N = 60;
    int coef = int(log2(N/base_nx) + 1);

    int x_size= base_nx*pow(2,int(log2(N/base_nx) + 1));
    int y_size =base_ny*pow(2,int(log2(N/base_ny)) + 1);


    int x_rect = base_nx_rect*pow(2,int(log2(N/base_nx) + 1));
    int y_rect = base_ny_rect*pow(2,int(log2(N/base_ny)) + 1);
    coef = nx / x_size;

    coef = max(1 , coef);

    if(coef<=1){
        x_size = nx;
        y_size = ny;
        x_rect = par.nx_rect;
        y_rect = par.ny_rect;
    }




    qDebug()<<x_size<<y_size<<coef;
    double hx_ = par.l2_new / x_size  , hy_ = par.l1_new / y_size;




    vector<Triangle> trio_apr;
    double cos_ = cos(alpha / (180.0 / M_PI)) , sin_ = sin(alpha / (180.0 / M_PI));

    int K = (x_size+1)*(y_size + 1) - x_rect*y_rect;

    int i,j;


    for( int k = 0; k < K;k++){
        get_ij(x_size, y_size, x_rect, y_rect, k,i,j);

        if( (i < x_size - x_rect && j < y_size) || (i >= x_size - x_rect && i < x_size && j < y_size - y_rect) ){
            trio_apr.push_back(Triangle(toNdc(QVector3D(i*hx_      + cos_/sin_* hy_*(j   )     , (x_copy[get_k(nx,ny,par.nx_rect,par.ny_rect ,coef*(i    ) ,coef*( j    )    )])     , hy_*(j   ) )) ,
                                        toNdc(QVector3D((i+1)*hx_  + cos_/sin_* hy_*(j+ 1)     , (x_copy[get_k(nx,ny,par.nx_rect ,par.ny_rect,coef*(i + 1) ,coef*( j + 1))])         , hy_*(j+ 1) )),
                                        toNdc(QVector3D((i+1)*hx_  + cos_/sin_* hy_*(j   )     , (x_copy[get_k(nx,ny,par.nx_rect,par.ny_rect ,coef*(i + 1) ,coef*( j    ) )])        , hy_*(j   ) ))));
            trio_apr.push_back(Triangle(toNdc(QVector3D(i*hx_      + cos_/sin_* hy_*(j   )     , (x_copy[get_k(nx,ny,par.nx_rect,par.ny_rect ,coef*(i    ) ,coef*( j    )    )])     , hy_*(j   ) )) ,
                                        toNdc(QVector3D(i*hx_      + cos_/sin_* hy_*(j+ 1)     , (x_copy[get_k(nx,ny,par.nx_rect,par.ny_rect ,coef*(i    ) ,coef*( j + 1)    )])     , hy_*(j+ 1) )) ,
                                        toNdc(QVector3D((i+1)*hx_  + cos_/sin_* hy_*(j+ 1)     , (x_copy[get_k(nx,ny,par.nx_rect ,par.ny_rect,coef*(i + 1) ,coef*( j + 1  ) )])        , hy_*(j+ 1) ))));
//            qDebug() << trio_apr[trio_apr.size() - 1] << trio_apr[trio_apr.size() - 2];
        }
    }

    return trio_apr;

}

MainWindow::~MainWindow()
{
    delete ui;
    //    freeThreadVars();
}

void MainWindow::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    DDD ddd(painter, *this, getEye());

//    const QPair<double, double> xRange = {0, ceil(par.l2 + par.l1_new*cos(par.alpha))};
//    const QPair<double, double> yRange = {func_min, func_max};
//    const QPair<double, double> zRange = {0, ceil(par.l1_new)};
    const QVector3D size = {3 , 3, 3};

    const QVector3D ex = {size.x(), 0, 0};
    const QVector3D ey = {0, size.y(), 0};
    const QVector3D ez = {0, 0, size.z()};

    const QVector3D a = -size / 2;
    const QVector3D b = a + ez;
    const QVector3D c = b + ex;
    const QVector3D d = c - ez;
    const QVector3D e = a + ey;

    const double xDiff = xRange.second - xRange.first;
    const double yDiff = yRange.second - yRange.first;
    const double zDiff = zRange.second - zRange.first;

    // переводит точку из системы координат графика в НСК
    auto toNdc = [&](const QVector3D& p) {
        float x = a.x() + (d.x() - a.x()) * ((p.x() - xRange.first) / xDiff);
        float y = a.y() + (e.y() - a.y()) * ((p.y() - yRange.first) / yDiff);
        float z = a.z() + (b.z() - a.z()) * ((p.z() - zRange.first) / zDiff);
        return QVector3D(x, y, z);
    };

    ddd.drawAxes(size, xRange, 5, yRange, 5, zRange, 5);
    QVector<QVector3D> parallelogram = {
        toNdc(par.cord_trans(par.xs[0] , par.ys[0], yRange.first)),
        toNdc(par.cord_trans(par.xs[0] , par.ys[ny], yRange.first)),
        toNdc(par.cord_trans(par.xs[par.nx] , par.ys[ny], yRange.first)),
        toNdc(par.cord_trans(par.xs[par.nx] , par.ys[0], yRange.first)),
    };
    ddd.fillPolygon(parallelogram, QColor(255, 128, 64, 64));
    QVector<QVector3D> cut = {
        toNdc(par.cord_trans(par.xs[par.nx - par.nx_rect] , par.ys[ny - par.ny_rect], yRange.first)),
        toNdc(par.cord_trans(par.xs[par.nx - par.nx_rect] , par.ys[ny], yRange.first)),
        toNdc(par.cord_trans(par.xs[par.nx] , par.ys[ny], yRange.first)),
        toNdc(par.cord_trans(par.xs[par.nx] , par.ys[ny - par.ny_rect], yRange.first)),
    };
    ddd.fillPolygon(cut, QColor(64, 128, 255, 64));

    painter.setPen(Qt::transparent);
   // ddd.drawTriangles(par.func_trio(getEye(), currentFunction(), xRange, yRange, zRange, size , base_nx, base_ny , base_nx_rect , base_ny_rect));
    auto eye = getEye();
    if (x_copy){

        std::sort(apr_trio.begin(), apr_trio.end(), [&](const Triangle& a, const Triangle& b) {
            return (eye - a.center()).length() > (eye - b.center()).length();
        });
        ddd.drawTriangles(apr_trio);
    }
    painter.setPen(Qt::black);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        lastMousePosition = event->pos();
        lastHorizontalAngle = horizontalAngle;
        lastVerticalAngle = verticalAngle;
        isRotating = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isRotating = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    const float sensitivity = 0.3;
    if (isRotating) {
        setHorizontalAngle(lastHorizontalAngle + (event->pos().x() - lastMousePosition.x()) * sensitivity);
        setVerticalAngle(lastVerticalAngle + (event->pos().y() - lastMousePosition.y()) * sensitivity);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    setZoom(zoom + event->delta() / 4000.0);
}

QVector3D MainWindow::getEye() {
    QMatrix4x4 rotator;
    rotator.rotate(horizontalAngle, {0, 1, 0});
    rotator.rotate(verticalAngle, {1, 0, 0});
    return rotator * QVector3D(0, 0.7, -2) / zoom;
}

MainWindow::XXX MainWindow::currentFunction() {
    return functions[currentFunctionIndex].first;
}

void MainWindow::setHorizontalAngle(int angle) {
    horizontalAngle = angle;
    redraw();
}

void MainWindow::setVerticalAngle(int angle) {
    verticalAngle = angle;
    redraw();
}

void MainWindow::setZoom(float zoom_) {
    zoom = qMax(0.3f, zoom_);
    zoom = qMin(2.0f, zoom);
    redraw();
}

void MainWindow::setParallelogram() {
    l1 = ui->l1LineEdit->text().toDouble();
    l2 = ui->l2LineEdit->text().toDouble();
    alpha = ui->alphaLineEdit->text().toDouble();
    k = ui->kLineEdit->text().toDouble();
    nx = ui->nxLineEdit->text().toInt();
    ny = ui->nyLineEdit->text().toInt();
    p = ui->pLineEdit->text().toInt();
    compute();
    funcRange();
}

void MainWindow::setCurrentFunction(int index) {
    currentFunctionIndex = index;
    ui->functionLabel->setText(functions[currentFunctionIndex].second);
    funcRange();
    compute();
    redraw();
}

void MainWindow::activate() {
    ui->computePushButton->setEnabled(true);
    ui->changeFunctionPushButton->setEnabled(true);
}

void MainWindow::deactivate() {
    ui->computePushButton->setEnabled(false);
    ui->changeFunctionPushButton->setEnabled(false);
}

void MainWindow::redraw() {
    update();
}

int MainWindow::compute() {
    deactivate();
    allocThreadVars();
    approx(p, tids, args);
    redraw();
    return 0;
}

void MainWindow::allocThreadVars() {
    //    freeThreadVars();

    error = new int(0);
    //par = parral(l1 , l2 , alpha, k , nx, ny);

    N = (nx + 1)*(ny + 1) - par.nx_rect*par.ny_rect;

    tids= new pthread_t[p];
    args= new Arg[p];
    buf = new double[p];
    x = new double[N];
    u = new double[N];
    v = new double[N];
    r = new double[N];

    memset(x , 0 , N*sizeof(double));
    memset(u , 0 , N*sizeof(double));
    memset(v , 0 , N*sizeof(double));
    memset(r , 0 , N*sizeof(double));
    memset(buf , 0 , p*sizeof(double));

    for(int i=0;i<p;i++){
        args[i].p=p;
        args[i].thr_ind=i;
        args[i].A=&a;
        args[i].b=&b;
        args[i].I=&I;
        args[i].x=x;
        args[i].u=u;
        args[i].v=v;
        args[i].r=r;
        args[i].buf = buf;
        args[i].error=error;
        args[i].nx = par.nx;
        args[i].ny = par.ny;
        args[i].f = currentFunction();
        args[i].par = &par;
    }
}

void MainWindow::freeThreadVars() {
    allThreadsPokushali = false;
    for(int i=0;i<p;i++) {
        pthread_join(tids[i],0);
    }
    swap(x , x_copy);
    apr_trio = func_apr_trio({3,3,3});
    redraw();
    delete[] tids;
    delete[] args;
    delete[] x;
    delete[] u;
    delete[] v;
    delete[] r;
    delete[] buf;
    delete error;

    tids = nullptr;
    args = nullptr;
    x = nullptr;
    u = nullptr;
    v = nullptr;
    r = nullptr;
    buf = nullptr;
    error = nullptr;
}
