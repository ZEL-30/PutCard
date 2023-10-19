#include "main_window.h"
#include "login.h"
#include "put-card/put_card.h"

#include <QCompleter>
#include <QDateTime>
#include <QDebug>
#include <QKeyEvent> //用于键盘事件
#include <QLineEdit>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(User *user, QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(new Ui_MainWindow)
    , order_(new Order)
    , user_(user) {
    ui_->setupUi(this);

    initWindow();

    initUi();

    initSignalSlot();

    refreshMainTab();
}

MainWindow::~MainWindow() {
    delete ui_;
    if (order_ != nullptr) {
        delete order_;
        order_ = nullptr;
    }
    if (user_ != nullptr) {
        delete user_;
        user_ = nullptr;
    }
}

void MainWindow::initWindow() {

    // 设置窗口标题
    setWindowTitle("星汉放卡工具");

    // 设置窗口图标
    setWindowIcon(QIcon(":/images/xh_logo.png"));
}

void MainWindow::initUi() {

    // 初始化放卡 tab
    initMainTab();

    // 初始化订单管理 tab
    initOrderTab();

    // 初始化用户管理 tab
    initUserTab();
}

void MainWindow::initMainTab() {
    ui_->current_user_edit->setDisabled(true);
    ui_->current_user_edit->setText(user_->currentUser().name);
    ui_->get_check_num_edit->setDisabled(true);

    ui_->get_order_name_combo->setEditable(true);
    ui_->get_order_name_combo->lineEdit()->setPlaceholderText("请输入订单号");
    ui_->get_order_name_combo->lineEdit()->setAlignment(Qt::AlignCenter);

    // 下拉框模糊匹配
    QCompleter *completer = new QCompleter(order_->nameList(), this);
    completer->setFilterMode(Qt::MatchContains);
    ui_->get_order_name_combo->setCompleter(completer);
}

void MainWindow::initOrderTab() {
    QStringList order_header = {"订单号", "校验位数", "创建时间"};
    initTable(ui_->order_table, order_header, order_header.size());
}

void MainWindow::initUserTab() {
    QStringList user_header = {"用户名", "权限"};
    initTable(ui_->user_table, user_header, user_header.size());
    // 设置权限下拉框内容
    for (int i = 0; i < 3; i++) {
        ui_->selected_combo_box->addItem(User::role_[i]);
    }
}

void MainWindow::initTable(QTableWidget *table, QStringList header, int col_count) {
    table->setRowCount(user_->users().size()); // 设置行数
    table->setColumnCount(col_count);          // 设置列数
    table->setHorizontalHeaderLabels(header);

    // 设置表格样式
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);   // 禁止编辑
    table->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选中整行
    table->setSelectionMode(QAbstractItemView::SingleSelection); // 只能选中一行
    table->setAlternatingRowColors(true);                        // 隔行变色

    // 设置表头字体
    QFont header_font;
    header_font.setPointSize(12);
    header_font.setBold(true);
    table->horizontalHeader()->setFont(header_font);
    table->verticalHeader()->setFont(header_font);

    // 设置表头内容颜色
    table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
    table->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");

    // 设置表格字体
    QFont font;
    font.setPointSize(12);
    table->setFont(font);

    // 设置表格行高和宽度， 序号列单独设置宽度
    table->verticalHeader()->setDefaultSectionSize(30);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置内容充满表格
    table->horizontalHeader()->setStretchLastSection(true);

    // 表格可以手动调整列宽
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // 设置表格内容居中
    table->setStyleSheet("QTableView::item {margin:3px 0px;}");
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // 设置表格内容颜色
    table->setStyleSheet("selection-background-color:lightblue;"); // 选中行背景色
    table->setStyleSheet("background-color:transparent;");         // 背景透明
    table->setStyleSheet("alternate-background-color:lightgray;"); // 隔行变色
    table->setStyleSheet("color:black;");                          // 字体颜色

    // 设置表格边框
    table->setFrameShape(QFrame::NoFrame);                 // 无边框
    table->setShowGrid(false);                             // 不显示网格线
    table->setStyleSheet("border:none;");                  // 边框颜色
    table->setStyleSheet("gridline-color:rgba(0,0,0,0);"); // 网格线颜色
}

void MainWindow::initSignalSlot() {
    // 切换用户按钮
    connect(ui_->switch_user_btn, &QPushButton::clicked, this, &MainWindow::switchUserBtnClicked);

    // 切换 tab
    connect(ui_->tabWidget, &QTabWidget::currentChanged, [=](int index) {
        switch (index) {

        // 主界面
        case 0:
            refreshMainTab();
            break;

        // 订单管理
        case 1:
            refreshOrderTab();
            break;

        // 用户管理
        case 2:
            refreshUserTab();
            break;

        default:
            break;
        }
    });

    // 主界面 Tab
    connect(ui_->get_order_name_combo, &QComboBox::currentTextChanged, this, &MainWindow::selectOrder);
    connect(ui_->target_line, &QLineEdit::returnPressed, this, &MainWindow::getTargetIccid);
    connect(ui_->before_line, &QLineEdit::returnPressed, this, &MainWindow::getBeforeIccid);
    connect(ui_->after_line, &QLineEdit::returnPressed, this, &MainWindow::getAfterIccid);

    // 订单配置 tab
    connect(ui_->order_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::showSelectedOrder);
    connect(ui_->add_order_btn, &QPushButton::clicked, this, &MainWindow::addOrderBtnClicked);
    connect(ui_->update_order_btn, &QPushButton::clicked, this, &MainWindow::updateOrderBtnClicked);
    connect(ui_->remove_order_btn, &QPushButton::clicked, this, &MainWindow::removeOrderBtnClicked);
    connect(ui_->clear_order_btn, &QPushButton::clicked, this, &MainWindow::clearOrderBtnClicked);

    // 用户配置 tab
    connect(ui_->user_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::showSelectedUser);
    connect(ui_->update_user_btn, &QPushButton::clicked, this, &MainWindow::updateUserBtnClicked);
    connect(ui_->add_user_btn, &QPushButton::clicked, this, &MainWindow::addUserBtnClicked);
    connect(ui_->remove_user_btn, &QPushButton::clicked, this, &MainWindow::removeUserBtnClicked);
    connect(ui_->clear_user_btn, &QPushButton::clicked, this, &MainWindow::clearUserBtnClicked);
}

void MainWindow::selectOrder() {
    ui_->get_check_num_edit->clear();

    // 获取订单号
    QString order_name = ui_->get_order_name_combo->currentText();
    if (!order_->currentOrder(order_name)) {
        return;
    }

    // 获取订单信息
    Order::OrderInfo order_info = order_->currentOrder();

    // 显示订单信息
    ui_->get_check_num_edit->setText(QString::number(order_info.check_num));
}

void MainWindow::getTargetIccid() {
    if (ui_->get_check_num_edit->text().isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择订单");
        ui_->target_line->clear();
        return;
    }

    target_iccid_ = ui_->target_line->text();
    ui_->before_line->setFocus();
}

void MainWindow::getBeforeIccid() {
    before_iccid_ = ui_->before_line->text();
    ui_->after_line->setFocus();
}

void MainWindow::getAfterIccid() {
    after_iccid_ = ui_->after_line->text();

    PutCard::CardInfo card_info{target_iccid_, before_iccid_, after_iccid_, order_->currentOrder().check_num};

    PutCard put_card(card_info);

    QString error, log_msg;
    if (put_card.check(error)) {
        log_msg = "用户[" + user_->currentUser().name + "] 前卡[" + before_iccid_ + "] 待放回卡[" + target_iccid_ +
            "] 后卡[" + after_iccid_ + "] 数据比对成功";
        log(order_->currentOrder().order_name, log_msg);

        ui_->target_line->clear();
        ui_->before_line->clear();
        ui_->after_line->clear();

        ui_->target_line->setFocus();
    } else {
        log_msg = "用户[" + user_->currentUser().name + "] 前卡[" + before_iccid_ + "] 待放回卡[" + target_iccid_ +
            "] 后卡[" + after_iccid_ + "] 数据比对失败";
        if (!log(order_->currentOrder().order_name, log_msg)) {
            printf("log write error\n");
        }

        QMessageBox::warning(this, "提示", "放卡失败: " + error);
        ui_->target_line->clear();
        ui_->before_line->clear();
        ui_->after_line->clear();

        ui_->target_line->setFocus();
    }
}

void MainWindow::switchUserBtnClicked() {
    // 关闭键盘监听
    this->releaseKeyboard();

    Login *login = new Login();
    login->show();
    this->close();
}

/// @brief 刷新主界面 tab
void MainWindow::refreshMainTab() {
    ui_->get_order_name_combo->clear();
    ui_->get_check_num_edit->clear();

    // 设置订单号下拉框内容
    for (int i = 0; i < order_->orders().size(); i++) {
        ui_->get_order_name_combo->addItem(order_->orders()[i].order_name);
    }

    // 下框默认不选中
    ui_->get_order_name_combo->setCurrentIndex(-1);

    if (user_->currentUser().role == 0) {
        ui_->get_order_name_combo->setEnabled(false);
        ui_->target_line->setEnabled(false);
        ui_->before_line->setEnabled(false);
        ui_->after_line->setEnabled(false);
    }
}

void MainWindow::addOrderBtnClicked() {
    // 获取订单信息
    QString          order_name  = ui_->selected_order_edit->text();
    int              check_num   = ui_->selected_checknum_edit->text().toInt();
    QString          create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    Order::OrderInfo new_order   = {order_name, check_num, create_time};

    if (order_name == "" || check_num == 0) {
        QMessageBox::warning(this, "添加失败", "订单号或校验位数不能为空");
        return;
    }

    // 添加订单
    if (order_->add(new_order)) {
        QMessageBox::information(this, "添加成功", "订单添加成功");
        refreshOrderTab();
    } else {
        QMessageBox::warning(this, "添加失败", "订单添加失败");
    }
}

void MainWindow::updateOrderBtnClicked() {
    // 获取选中的行索引
    int index = ui_->order_table->currentRow();

    // 获取订单信息
    QString          order_name  = ui_->selected_order_edit->text();
    int              check_num   = ui_->selected_checknum_edit->text().toInt();
    QString          create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    Order::OrderInfo new_order   = {order_name, check_num, create_time};

    if (order_name == "" || check_num == 0 || index == -1) {
        QMessageBox::warning(this, "修改失败", "订单号或校验位数不能为空");
        return;
    }

    // 保存订单信息
    if (order_->update(order_->orders()[index].order_name, new_order)) {
        QMessageBox::information(this, "保存成功", "订单信息保存成功");
        refreshOrderTab();
    } else {
        QMessageBox::warning(this, "保存失败", "订单信息保存失败");
    }
}

void MainWindow::removeOrderBtnClicked() {

    // 获取选中的行索引
    int index = ui_->order_table->currentRow();

    if (index == -1) {
        QMessageBox::warning(this, "删除失败", "请选中要删除的订单");
        return;
    }

    // 弹窗确认
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除订单", "确认删除 [" + order_->orders()[index].order_name + "] 吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        refreshOrderTab();
        return;
    }

    // 删除订单
    if (order_->remove(order_->orders()[index].order_name)) {
        QMessageBox::information(this, "删除成功", "订单删除成功");
        refreshOrderTab();
    } else {
        QMessageBox::warning(this, "删除失败", "订单删除失败");
    }
}

void MainWindow::clearOrderBtnClicked() {
    // 弹窗确认
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "清空订单", "确认清空所有订单吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        refreshOrderTab();
        return;
    }

    if (order_->clear()) {
        QMessageBox::information(this, "清空成功", "订单清空成功");
        refreshOrderTab();
    } else {
        QMessageBox::warning(this, "清空失败", "订单清空失败");
    }
}

void MainWindow::showSelectedOrder() {
    // 获取选中的行索引
    int index = ui_->order_table->currentRow();

    ui_->selected_order_edit->setText(order_->orders()[index].order_name);
    ui_->selected_checknum_edit->setText(QString::number(order_->orders()[index].check_num));
}

void MainWindow::refreshOrderTab() {
    // 获取订单管理 tableWidget
    ui_->order_table->setRowCount(order_->orders().size()); // 设置行数
    ui_->order_table->setColumnCount(3);                    // 设置列数
    QStringList header = {"订单号", "校验位数", "创建时间"};
    ui_->order_table->setHorizontalHeaderLabels(header);

    // 设置表格内容
    for (int i = 0; i < order_->orders().size(); i++) {
        ui_->order_table->setItem(i, 0, new QTableWidgetItem(order_->orders()[i].order_name));
        ui_->order_table->setItem(i, 1, new QTableWidgetItem(QString::number(order_->orders()[i].check_num)));
        ui_->order_table->setItem(i, 2, new QTableWidgetItem(order_->orders()[i].create_time));
    }

    ui_->selected_order_edit->clear();
    ui_->selected_checknum_edit->clear();

    if (user_->currentUser().role == 1) {
        ui_->clear_order_btn->setEnabled(false);
    } else if (user_->currentUser().role == 2) {
        ui_->add_order_btn->setEnabled(false);
        ui_->remove_order_btn->setEnabled(false);
        ui_->update_order_btn->setEnabled(false);
        ui_->clear_order_btn->setEnabled(false);

        ui_->selected_order_edit->setEnabled(false);
        ui_->selected_checknum_edit->setEnabled(false);
    }
}

void MainWindow::addUserBtnClicked() {
    // 获取用户信息
    QString        name     = ui_->selected_user_edit->text();
    QString        password = ui_->selected_password_edit->text();
    int            role     = ui_->selected_combo_box->currentIndex();
    User::UserInfo new_user = {name, password, (User::Role) role};

    if (name == "" || password == "") {
        QMessageBox::warning(this, "添加失败", "用户名或密码不能为空");
        return;
    }

    // 添加用户
    if (user_->add(new_user)) {
        QMessageBox::information(this, "添加成功", "用户添加成功");
        refreshUserTab();
    } else {
        QMessageBox::warning(this, "添加失败", "用户添加失败");
    }
}

void MainWindow::updateUserBtnClicked() {
    // 获取选中的行索引
    int index = ui_->user_table->currentRow();

    // 获取用户信息
    QString        name     = ui_->selected_user_edit->text();
    QString        password = ui_->selected_password_edit->text();
    int            role     = ui_->selected_combo_box->currentIndex();
    User::UserInfo new_user = {name, password, (User::Role) role};

    if (name == "" || password == "" || index == -1) {
        QMessageBox::warning(this, "修改失败", "用户名或密码不能为空");
        return;
    }

    // 保存用户信息
    if (user_->update(user_->users()[index].name, new_user)) {
        QMessageBox::information(this, "保存成功", "用户信息保存成功");
        refreshUserTab();
    } else {
        QMessageBox::warning(this, "保存失败", "用户信息保存失败");
    }
}

void MainWindow::removeUserBtnClicked() {

    // 获取选中的行索引
    int index = ui_->user_table->currentRow();

    if (index == -1) {
        QMessageBox::warning(this, "删除失败", "请选中要删除的用户");
        return;
    }

    // 弹窗确认
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除用户", "确认删除 [" + user_->users()[index].name + "] 吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        refreshUserTab();
        return;
    }

    // 删除用户
    if (user_->remove(user_->users()[index].name)) {
        QMessageBox::information(this, "删除成功", "用户删除成功");
        refreshUserTab();
    } else {
        QMessageBox::warning(this, "删除失败", "用户删除失败");
    }
}

void MainWindow::clearUserBtnClicked() {
    // 弹窗确认
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "清空用户", "确认清空所有用户吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        refreshUserTab();
        return;
    }

    if (user_->clear()) {
        QMessageBox::information(this, "清空成功", "用户清空成功");
        refreshUserTab();
    } else {
        QMessageBox::warning(this, "清空失败", "用户清空失败");
    }
}

void MainWindow::showSelectedUser() {

    // 获取选中的行索引
    int index = ui_->user_table->currentRow();

    ui_->selected_user_edit->setText(user_->users()[index].name);
    ui_->selected_password_edit->setText(user_->users()[index].password);
    ui_->selected_combo_box->setCurrentIndex((int) user_->users()[index].role);
}

void MainWindow::refreshUserTab() {
    // 获取用户管理 tableWidget
    ui_->user_table->setRowCount(user_->users().size()); // 设置行数
    ui_->user_table->setColumnCount(2);                  // 设置列数
    QStringList header = {"用户名", "权限"};
    ui_->user_table->setHorizontalHeaderLabels(header);

    // 设置表格内容
    for (int i = 0; i < user_->users().size(); i++) {
        ui_->user_table->setItem(i, 0, new QTableWidgetItem(user_->users()[i].name));
        int role = (int) user_->users()[i].role;
        ui_->user_table->setItem(i, 1, new QTableWidgetItem(User::role_[role]));
    }

    ui_->selected_user_edit->clear();
    ui_->selected_password_edit->clear();
    ui_->selected_combo_box->setCurrentIndex(-1);

    if (user_->currentUser().role == 1) {
        ui_->clear_user_btn->setEnabled(false);
    } else if (user_->currentUser().role == 2) {
        ui_->add_user_btn->setEnabled(false);
        ui_->remove_user_btn->setEnabled(false);
        ui_->update_user_btn->setEnabled(false);
        ui_->clear_user_btn->setEnabled(false);

        ui_->selected_user_edit->setEnabled(false);
        ui_->selected_password_edit->setEnabled(false);
        ui_->selected_combo_box->setEnabled(false);
    }
}

bool MainWindow::log(const QString &filename, const QString &msg) {

    // 获取当前时间
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 获取当前程序所在路径
    QString current_path = QCoreApplication::applicationDirPath();

    // 获取日志文件路径
    QString log_path = current_path + "/log/" + filename + ".log";

    // 打开日志文件
    QFile file(log_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        return false;
    }

    // 写入日志
    QTextStream text_stream(&file);
    text_stream << current_date_time << " " << msg << "\r\n";
    file.flush();
    file.close();

    return true;
}