#include "put_card.h"

#include <QDebug>

PutCard::PutCard(const CardInfo &card_info)
    : card_info_(card_info) {}

PutCard::~PutCard() {}

bool PutCard::check(QString &error) {

    // 数据转换
    if (!dataConvert(error)) {
        return false;
    }

    // 判断 target_iccid_ 是否连续
    if (target_iccid_ != before_iccid_ + 1) {
        error = "target_iccid is not continuous";
        return false;
    }

    // 判断 after_iccid_ 是否连续
    if (after_iccid_ != target_iccid_ + 1) {
        error = "after_iccid is not continuous";
        return false;
    }

    return true;
}

bool PutCard::dataConvert(QString &error) {

    int check_num = card_info_.check_num;

    // 判断 check_num 是否大于 target_iccid位数
    if (check_num > card_info_.target_iccid.size()) {
        error = "check_num is bigger than target_iccid size";
        return false;
    }

    if (card_info_.target_iccid.left(5).toInt() != card_info_.after_iccid.left(5).toInt() ||
        card_info_.target_iccid.left(5).toInt() != card_info_.before_iccid.left(5).toInt()) {
        error = "target_iccid, after_iccid and after_iccid is not same";
        return false;
    }

    // 数据转换
    target_iccid_ = card_info_.target_iccid.mid(5, check_num - 5).toLongLong();
    before_iccid_ = card_info_.before_iccid.mid(5, check_num - 5).toLongLong();
    after_iccid_  = card_info_.after_iccid.mid(5, check_num - 5).toLongLong();

    return true;
}