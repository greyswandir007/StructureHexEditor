#include "jsonhighlighter.h"
#include "jsonstoreddata.h"

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter (parent) {
    HighlightingRule highlightingRule;
    // Value numbers
   highlightingRule.pattern = QRegularExpression("([-0-9.]+)(?!([^\"]*\"[\\s]*\\:))");
   highlightingRule.format.setForeground(Qt::darkRed);
   highlightingRules.append(highlightingRule);

   // Value string
   highlightingRule.pattern = QRegularExpression("(\"[^\"]*\")");
   highlightingRule.format.setForeground(Qt::darkGreen);
   highlightingRules.append(highlightingRule);

   // Param
   highlightingRule.pattern = QRegularExpression("(\"[^\"]*\")\\s*\\:");
   highlightingRule.format.setForeground(Qt::darkBlue);
   highlightingRules.append(highlightingRule);

   // Hex number
   highlightingRule.pattern = QRegularExpression(":+(?:[: []*)(\"0x[0-9A-Fa-f]*\")");
   highlightingRule.format.setForeground(Qt::darkRed);
   highlightingRules.append(highlightingRule);

   // Root params:
   highlightingRule.pattern = QRegularExpression("(\"extesions\")\\s*\\:");
   highlightingRule.format.setForeground(Qt::darkYellow);
   highlightingRule.format.setFontWeight(QFont::Bold);
   highlightingRules.append(highlightingRule);

   highlightingRule.pattern = QRegularExpression("(\"signature\")\\s*\\:");
   highlightingRule.format.setForeground(Qt::darkYellow);
   highlightingRule.format.setFontWeight(QFont::Bold);
   highlightingRules.append(highlightingRule);

   // Keywords:
   for (auto keyword : keywords) {
       highlightingRule.pattern = QRegularExpression(QString("(\"%1\")\\s*\\:").arg(keyword));
       highlightingRule.format.setForeground(Qt::darkCyan);
       highlightingRule.format.setFontWeight(QFont::Bold);
       highlightingRules.append(highlightingRule);
   }

   // Types:
   for (auto type: jsonTypes) {
      highlightingRule.pattern = QRegularExpression(QString(":+(?:[: []*)(\"%1\")").arg(type));
      highlightingRule.format.setForeground(Qt::darkMagenta);
      highlightingRule.format.setFontWeight(QFont::Bold);
      highlightingRules.append(highlightingRule);
   }
}

void JsonHighlighter::highlightBlock(const QString &text) {
    for (auto rule: highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}