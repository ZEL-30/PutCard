#pragma once

#include <QString>

class PutCard {

  public:
    struct CardInfo {
        QString target_iccid;
        QString before_iccid;
        QString after_iccid;
        int     check_num;
    };

    PutCard(const CardInfo &card_info);

    ~PutCard();

    /// @brief 校验
    /// @param error 传出参数，错误信息
    bool check(QString &error);

  private:
    /// @brief 数据转换
    /// @param error 传出参数，错误信息
    bool dataConvert(QString &error);

  private:
    CardInfo  card_info_;
    qlonglong target_iccid_;
    qlonglong before_iccid_;
    qlonglong after_iccid_;
};