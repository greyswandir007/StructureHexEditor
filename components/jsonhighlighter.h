#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>



class JsonHighlighter : public QSyntaxHighlighter {
public:
    JsonHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct  HighlightingRule  {
        QRegularExpression pattern;
        QTextCharFormat format;
        int startOffset = 0;
        int lengthReduce = 0;
    };
    QVector<HighlightingRule> highlightingRules;

    QList<QString> keywords = {"type", "size", "offset", "itemSize", "displayName", "displayNameRef", "fields", "item"};
};

#endif // JSONHIGHLIGHTER_H
