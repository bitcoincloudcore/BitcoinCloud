// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINCLOUD_QT_BITCOINCLOUDADDRESSVALIDATOR_H
#define BITCOINCLOUD_QT_BITCOINCLOUDADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class BitcoinCloudAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit BitcoinCloudAddressEntryValidator(QObject* parent);

    State validate(QString& input, int& pos) const;
};

/** BitcoinCloud address widget validator, checks for a valid bitcoincloud address.
 */
class BitcoinCloudAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit BitcoinCloudAddressCheckValidator(QObject* parent);

    State validate(QString& input, int& pos) const;
};

#endif // BITCOINCLOUD_QT_BITCOINCLOUDADDRESSVALIDATOR_H
