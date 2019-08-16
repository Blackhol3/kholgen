#include "TransparentItemDelegate.h"

#include <QPainter>

TransparentItemDelegate::TransparentItemDelegate(int const alpha, QObject *parent): QStyledItemDelegate(parent), selectionColor(Qt::darkBlue)
{
	selectionColor.setAlpha(alpha);
}

void TransparentItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	auto unselectedOption = option;
	unselectedOption.state.setFlag(QStyle::State_Selected, false);

	if (option.state.testFlag(QStyle::State_Selected)) {
		painter->fillRect(option.rect, selectionColor);
	}

	QStyledItemDelegate::paint(painter, unselectedOption, index);
}
