#include <QtGui/QVBoxLayout>
#include "listWidget.h"

using namespace qReal;

ListWidget::ListWidget(QWidget *parent)
	: QWidget(parent)
	, mListWidget(new QListWidget(this))
	, mOkButton(new QPushButton(tr("&OK"), this))
{
	mOkButton->setDisabled(true);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mListWidget);
	mainLayout->addWidget(mOkButton);

	setLayout(mainLayout);

	connect(mListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(okActivate()));
	connect(mOkButton, SIGNAL(clicked()), this, SLOT(okButtonHandler()));
	connect(mListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
			this, SLOT(doubleClickHandler(QListWidgetItem*)));
}

void ListWidget::addItem(QString const &text, QString const &userData, QString const &toolTip)
{
	QListWidgetItem *currentItem = new QListWidgetItem(text, mListWidget);
	currentItem->setData(Qt::UserRole, userData);
	currentItem->setToolTip(toolTip);
	mListWidget->addItem(currentItem);
}

void ListWidget::okButtonHandler()
{
	emit userDataSelected(userData(mListWidget->currentItem()));
}

void ListWidget::doubleClickHandler(QListWidgetItem *item)
{
	emit userDataSelected(userData(item));
}

QString ListWidget::userData(QListWidgetItem *item)
{
	return item->data(Qt::UserRole).toString();
}

void ListWidget::okActivate()
{
	mOkButton->setEnabled(true);
}