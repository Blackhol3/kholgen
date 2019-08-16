#pragma once

#include <QStyledItemDelegate>

class TransparentItemDelegate: public QStyledItemDelegate
{
	public:
		TransparentItemDelegate(int const alpha, QObject *parent = nullptr);
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	protected:
		QColor selectionColor;
};
