// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/bitcoincloudunits.h>

#include <primitives/transaction.h>

#include <QStringList>

BitcoinCloudUnits::BitcoinCloudUnits(QObject* parent) : QAbstractListModel(parent),
                                                        unitlist(availableUnits())
{
}

QList<BitcoinCloudUnits::Unit> BitcoinCloudUnits::availableUnits()
{
    QList<BitcoinCloudUnits::Unit> unitlist;
    unitlist.append(BCL);
    unitlist.append(mBCL);
    unitlist.append(uBCL);
    unitlist.append(SAT);
    return unitlist;
}

bool BitcoinCloudUnits::valid(int unit)
{
    switch (unit) {
    case BCL:
    case mBCL:
    case uBCL:
    case SAT:
        return true;
    default:
        return false;
    }
}

QString BitcoinCloudUnits::longName(int unit)
{
    switch (unit) {
    case BCL:
        return QString("BCL");
    case mBCL:
        return QString("mBCL");
    case uBCL:
        return QString::fromUtf8("µBCL (bits)");
    case SAT:
        return QString("Satoshi (sat)");
    default:
        return QString("???");
    }
}

QString BitcoinCloudUnits::shortName(int unit)
{
    switch (unit) {
    case uBCL:
        return QString::fromUtf8("bits");
    case SAT:
        return QString("sat");
    default:
        return longName(unit);
    }
}

QString BitcoinCloudUnits::description(int unit)
{
    switch (unit) {
    case BCL:
        return QString("BitcoinClouds");
    case mBCL:
        return QString("Milli-BitcoinClouds (1 / 1" THIN_SP_UTF8 "000)");
    case uBCL:
        return QString("Micro-BitcoinClouds (bits) (1 / 1" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
    case SAT:
        return QString("Satoshi (sat) (1 / 100" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
    default:
        return QString("???");
    }
}

qint64 BitcoinCloudUnits::factor(int unit)
{
    switch (unit) {
    case BCL:
        return 100000000;
    case mBCL:
        return 100000;
    case uBCL:
        return 100;
    case SAT:
        return 1;
    default:
        return 100000000;
    }
}

int BitcoinCloudUnits::decimals(int unit)
{
    switch (unit) {
    case BCL:
        return 8;
    case mBCL:
        return 5;
    case uBCL:
        return 2;
    case SAT:
        return 0;
    default:
        return 0;
    }
}

QString BitcoinCloudUnits::format(int unit, const CAmount& nIn, bool fPlus, SeparatorStyle separators)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if (!valid(unit))
        return QString(); // Refuse to format invalid unit
    qint64 n = (qint64)nIn;
    qint64 coin = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    QString quotient_str = QString::number(quotient);

    // Use SI-style thin space separators as these are locale independent and can't be
    // confused with the decimal marker.
    QChar thin_sp(THIN_SP_CP);
    int q_size = quotient_str.size();
    if (separators == separatorAlways || (separators == separatorStandard && q_size > 4))
        for (int i = 3; i < q_size; i += 3)
            quotient_str.insert(q_size - i, thin_sp);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');

    if (num_decimals > 0) {
        qint64 remainder = n_abs % coin;
        QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');
        return quotient_str + QString(".") + remainder_str;
    } else {
        return quotient_str;
    }
}


// NOTE: Using formatWithUnit in an HTML context risks wrapping
// quantities at the thousands separator. More subtly, it also results
// in a standard space rather than a thin space, due to a bug in Qt's
// XML whitespace canonicalisation
//
// Please take care to use formatHtmlWithUnit instead, when
// appropriate.

QString BitcoinCloudUnits::formatWithUnit(int unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    return format(unit, amount, plussign, separators) + QString(" ") + shortName(unit);
}

QString BitcoinCloudUnits::formatHtmlWithUnit(int unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    QString str(formatWithUnit(unit, amount, plussign, separators));
    str.replace(QChar(THIN_SP_CP), QString(THIN_SP_HTML));
    return QString("<span style='white-space: nowrap;'>%1</span>").arg(str);
}


bool BitcoinCloudUnits::parse(int unit, const QString& value, CAmount* val_out)
{
    if (!valid(unit) || value.isEmpty())
        return false; // Refuse to parse invalid unit or empty string
    int num_decimals = decimals(unit);

    // Ignore spaces and thin spaces when parsing
    QStringList parts = removeSpaces(value).split(".");

    if (parts.size() > 2) {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if (parts.size() > 1) {
        decimals = parts[1];
    }
    if (decimals.size() > num_decimals) {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if (str.size() > 18) {
        return false; // Longer numbers will exceed 63 bits
    }
    CAmount retvalue(str.toLongLong(&ok));
    if (val_out) {
        *val_out = retvalue;
    }
    return ok;
}

QString BitcoinCloudUnits::getAmountColumnTitle(int unit)
{
    QString amountTitle = QObject::tr("Amount");
    if (BitcoinCloudUnits::valid(unit)) {
        amountTitle += " (" + BitcoinCloudUnits::shortName(unit) + ")";
    }
    return amountTitle;
}

int BitcoinCloudUnits::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant BitcoinCloudUnits::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row >= 0 && row < unitlist.size()) {
        Unit unit = unitlist.at(row);
        switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(longName(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}

CAmount BitcoinCloudUnits::maxMoney()
{
    return MAX_MONEY;
}
