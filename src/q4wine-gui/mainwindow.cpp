/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Malakhov Alexey                           *
 *   brezerk@gmail.com                                                     *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#include "mainwindow.h"

MainWindow::MainWindow(int startState, QWidget * parent, Qt::WFlags f) : QMainWindow(parent, f){

	// Loading libq4wine-core.so
	libq4wine.setFileName("libq4wine-core");

	if (!libq4wine.load()){
		libq4wine.load();
	}

	// Getting corelib calss pointer
	CoreLibClassPointer = (CoreLibPrototype *) libq4wine.resolve("createCoreLib");
	CoreLib.reset((corelib *)CoreLibClassPointer(true));

	clearTmp();
	// Base GUI setup
	setupUi(this);

	if (startState == 1)
		this->showMinimized();

	setWindowTitle(tr("%1 :. Qt4 GUI for Wine v%2").arg(APP_NAME) .arg(APP_VERS));

	std::auto_ptr<DragListWidget> lstIcons (new DragListWidget(THEME_NAME, tab));
	connect(this, SIGNAL(showFolderContents(QString, QString, QString)), lstIcons.get(), SLOT(showFolderContents(QString, QString, QString)));
	connect(lstIcons.get(), SIGNAL(iconItemClick(QString, QString, QString, QString, QString)), this, SLOT(lstIcons_ItemClick(QString, QString, QString, QString, QString)));
	connect(lstIcons.get(), SIGNAL(changeStatusText(QString)), this, SLOT(changeStatusText(QString)));

	std::auto_ptr<QWidget> wid (new QWidget(tab));
	std::auto_ptr<QVBoxLayout> vlayout (new QVBoxLayout);

	vlayout ->addWidget(widgetFilter);
	vlayout ->addWidget(lstIcons.release());
	vlayout ->setMargin(0);
	wid->setLayout(vlayout.release());

	splitter.reset(new QSplitter(tab));
	splitter->addWidget(twPrograms);
	splitter->addWidget(wid.release());

	vlayout.reset(new QVBoxLayout);
	vlayout->addWidget(splitter.get());
	vlayout->addWidget(gbInfo);
	vlayout->setMargin(3);
	tab->setLayout(vlayout.release());

	// Updating database connected items
	updateDtabaseConnectedItems();

	// Getting settings from config file
	this->getSettings();

	timer.reset(new QTimer());
	// Timer flag to running
	_IS_TIMER_RUNNING=TRUE;

	// Connecting signals and slots
	connect(timer.get(), SIGNAL(timeout()), this, SLOT(getWineProccessInfo()));
	connect(tbwGeneral, SIGNAL(currentChanged(int)), this, SLOT(CoreFunction_ResizeContent(int)));
	connect(cmdManagePrefixes, SIGNAL(clicked()), this, SLOT(cmdManagePrefixes_Click()));
	connect(cmdCreateFake, SIGNAL(clicked()), this, SLOT(cmdCreateFake_Click()));
	connect(cmdUpdateFake, SIGNAL(clicked()), this, SLOT(cmdUpdateFake_Click()));
	connect(cmdWinetricks, SIGNAL(clicked()), this, SLOT(cmdWinetricks_Click()));
	connect(cmdClearFilter, SIGNAL(clicked()), this, SLOT(cmdClearFilter_Click()));
	connect(cmdTestWis, SIGNAL(clicked()), this, SLOT(cmdTestWis_Click()));
	connect(cbPrefixes, SIGNAL(currentIndexChanged(QString)), this, SLOT(cbPrefixes_Change(QString)));

	// Signals commection for Icons and Folders
	connect(twPrograms, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(twPrograms_ItemClick(QTreeWidgetItem *, int)));
	connect(twPrograms, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(twPrograms_ShowContextMenu(const QPoint &)));
	/*connect(lstIcons.get(), SIGNAL(itemDoubleClicked (QListWidgetItem *)), this, SLOT(lstIcons_ItemDoubleClick(QListWidgetItem *)));
	connect(lstIcons.get(), SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(lstIcons_ItemClick(QListWidgetItem *)));
	connect(lstIcons.get(), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(lstIcons_ShowContextMenu(const QPoint &)));
	connect(lstIcons.get(), SIGNAL(startDrag ()), this, SLOT(startDrag()));
	connect(lstIcons.get(), SIGNAL(startDrop(QList<QUrl>)), this, SLOT(startDrop(QList<QUrl>)));
	*/

	// Signals for updating toolbars
	connect(tableProc, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(tableProc_ShowContextMenu(const QPoint &)));
	connect(tableProc, SIGNAL(clicked(const QModelIndex &)), this, SLOT(tablePrefix_UpdateContentList(const QModelIndex &)));

	// Init and connect SLOT & SIGNALS for context menus
	connect(tablePrefix, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(tablePrefix_ShowContextMenu(const QPoint &)));
	connect(tablePrefix, SIGNAL(clicked(const QModelIndex &)), this, SLOT(tablePrefix_UpdateContentList(const QModelIndex &)));

	//Main menu actions connection to slots
	connect(mainRun, SIGNAL(triggered()), this, SLOT(mainRun_Click()));
	connect(mainPrograms, SIGNAL(triggered()), this, SLOT(mainPrograms_Click()));
	connect(mainProcess, SIGNAL(triggered()), this, SLOT(mainProcess_Click()));
	connect(mainSetup, SIGNAL(triggered()), this, SLOT(mainSetup_Click()));
	connect(mainPrefix, SIGNAL(triggered()), this, SLOT(mainPrefix_Click()));
	connect(mainImageManage, SIGNAL(triggered()), this, SLOT(mainImageManager_Click()));
	connect(mainAbout, SIGNAL(triggered()), this, SLOT(mainAbout_Click()));
	connect(mainAboutQt, SIGNAL(triggered()), this, SLOT(mainAboutQt_Click()));
	connect(mainExportIcons, SIGNAL(triggered()), this, SLOT(mainExportIcons_Click()));
	connect(mainFirstSteps, SIGNAL(triggered()), this, SLOT(mainFirstSteps_Click()));
	connect(mainFAQ, SIGNAL(triggered()), this, SLOT(mainFAQ_Click()));
	connect(mainIndex, SIGNAL(triggered()), this, SLOT(mainIndex_Click()));
	connect(mainWebsite, SIGNAL(triggered()), this, SLOT(mainWebsite_Click()));
	connect(mainDonate, SIGNAL(triggered()), this, SLOT(mainDonate_Click()));
	connect(mainBugs, SIGNAL(triggered()), this, SLOT(mainBugs_Click()));


	connect(txtIconFilter, SIGNAL(textChanged(QString)), this, SLOT(txtIconFilter_textChanged(QString)));

#ifndef WITH_ICOUTILS
	mainExportIcons->setEnabled(false);
#endif

#ifdef WITH_DEVELOP_STUFF
	cmdTestWis->setEnabled(true);
#else
	cmdTestWis->setEnabled(false);
#endif

	connect(mainOptions, SIGNAL(triggered()), this, SLOT(mainOptions_Click()));
	connect(mainInstall, SIGNAL(triggered()), this, SLOT(mainInstall_Click()));
	connect(mainExit, SIGNAL(triggered()), this, SLOT(mainExit_Click()));

	// Setting context menu policy
	tableProc->setContextMenuPolicy(Qt::CustomContextMenu);
	twPrograms->setContextMenuPolicy(Qt::CustomContextMenu);
	tablePrefix->setContextMenuPolicy(Qt::CustomContextMenu);
	//lstIcons->setContextMenuPolicy(Qt::CustomContextMenu);

	// Creating actions for context menus & toolbars
	createMenuActions();
	createToolBarActions();

	// Enveropment path initialization
	HOME_PATH = QDir::homePath();
	ROOT_PATH = QDir::rootPath();
	TEMP_PATH = QDir::tempPath();
	WINE_DEFAULT_PREFIX.append(HOME_PATH);
	WINE_DEFAULT_PREFIX.append("/.wine");

	// Setting default IconsSize for lstIcons (Wine-Programm-Menu) need for user settings

	twPrograms->installEventFilter(this);

	txtIconFilter->installEventFilter(this);
	installEventFilter(this);
	cmdClearFilter->installEventFilter(this);

	// FIXME: Move this into shared libaray
	runAutostart();
	createTrayIcon();

	// Creating AppDBScrollWidget and place it into frameAppDBWidget layout
	appdbWidget.reset(new AppDBWidget());
	connect (this, SIGNAL(appdbWidget_startSearch(short int, QString)), appdbWidget.get(), SLOT(itemTrigged(short int, QString)));
	connect (appdbWidget.get(), SIGNAL(xdgOpenUrl(QString)), this, SLOT(xdgOpenUrl(QString)));

	frameAppDBWidgetLayout->addWidget(appdbWidget.release());

	connect (cmdAppDBSearch, SIGNAL(clicked()), this, SLOT(cmdAppDBSearch_Click()));

	return;
}

void MainWindow::txtIconFilter_textChanged(QString){
	twPrograms_ItemClick(twPrograms->currentItem(), 0);
}

void MainWindow::clearTmp(){
	QString fileName = QDir::homePath();
	fileName.append("/.config/");
	fileName.append(APP_SHORT_NAME);
	fileName.append("/tmp/");

	QDir dir(fileName);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		QFile(list.at(i).absoluteFilePath()).remove();
	}
	return;
}

void MainWindow::startDrop(QList<QUrl> files){
	std::auto_ptr<QTreeWidgetItem> treeItem (twPrograms->currentItem());

	if (!treeItem.get())
		return;

	bool ok=false;
	QString file="", fileName="";
	QStringList list1;

	QString dir_name="", prefix_name="";

	if (treeItem->parent()){
		prefix_name = treeItem->parent()->text(0);
		dir_name = treeItem->text(0);
	} else {
		prefix_name = treeItem->text(0);
		dir_name = "";
	}

	for (int i=0; i < files.count(); i++){
		if (files.at(i).toLocalFile().contains(".exe", Qt::CaseInsensitive) || files.at(i).toLocalFile().contains(".bat", Qt::CaseInsensitive) || files.at(i).toLocalFile().contains(".com", Qt::CaseInsensitive)){
			file = files.at(i).toLocalFile();
			list1 = file.split("/");
			fileName=list1.last().left(list1.last().length() - list1.last().split(".").last().length() - 1);

			while (db_icon.isExistsByName(prefix_name, dir_name, fileName)){
				fileName = QInputDialog::getText(this, tr("Sorry. It seems icon already exists."), tr("Sorry. It seems icon already exists.<br>Please choose another name, or cancel operation."), QLineEdit::Normal, fileName , &ok);
				if (!ok){
					treeItem.release();
					return;
				}
			}

			if (files.at(i).toLocalFile().contains(".bat", Qt::CaseInsensitive)){
				file = "--backend=user ";
				file.append(CoreLib->getWinePath(files.at(i).toLocalFile(), "-w"));
				db_icon.addIcon(file, "wineconsole", "", "", prefix_name, dir_name, fileName, "", "", "", "", file.left(file.length() - file.split("/").last().length()), "", 0);
			} else {
				db_icon.addIcon("", file, "", "", prefix_name, dir_name, fileName, "", "", "", "", file.left(file.length() - file.split("/").last().length()), "", 0);
			}
		}
	}
	twPrograms_ItemClick(treeItem.release(), 0);
}

void MainWindow::startDrag (){
	/*
	QString fileName;
	QList<QListWidgetItem *> items = lstIcons->selectedItems ();

	if (items.count()>0){
		std::auto_ptr<QTreeWidgetItem> treeItem (twPrograms->currentItem());

		if (!treeItem.get())
			return;

		std::auto_ptr<QMimeData> mimeData(new QMimeData());
		QList<QUrl> urls;

		for (int i=0; i<items.count(); i++){
			if (treeItem->parent()){
				fileName = CoreLib->createDesktopFile(treeItem->parent()->text(0), treeItem->text(0), items.at(i)->text());
			} else {
				fileName = CoreLib->createDesktopFile(treeItem->text(0), "", items.at(i)->text());
			}
			urls<<QUrl::fromLocalFile(fileName);
		}
		mimeData->setUrls(urls);
		std::auto_ptr<QDrag> drag(new QDrag(this));
		drag->setMimeData(mimeData.release());
		drag->setPixmap(items.at(0)->icon().pixmap(32));
		drag->start(Qt::MoveAction);
		drag.release();
		treeItem.release();
	}
	*/
	return;
}

void MainWindow::cmdTestWis_Click(){
	return;
}

void MainWindow::cmdAppDBSearch_Click(){
	if (!cbSearchText->currentText().isEmpty())
		emit(appdbWidget_startSearch(1, cbSearchText->currentText()));

	return;
}

void MainWindow::cmdWinetricks_Click() {
#ifndef WITH_WINETRIKS
	QMessageBox::warning(this, tr("Warning"), tr("<p>q4wine was compiled without winetriks support.</p><p>If you wish to enable winetriks support add:</p><p> \"-DWITH_WINETRIKS=ON\" to cmake arguments.</p>"));
#else
	QMessageBox::warning(this, tr("Warning"), tr("<p>Winetricks officaly NOT supported by q4wine.</p><p>There was some repports about bugs, slows and errors on winetriks and q4wine usage at same time.</p>"));

	if (CoreLib->getSetting("console", "bin").toString().isEmpty()){
		QMessageBox::warning(this, tr("Error"), tr("<p>You do not set default console binary.</p><p>Set it into q4wine option dialog.</p>"));
		return;
	}
	winetricks triks(cbPrefixes->currentText());
	triks.exec();
#endif

	return;
}

void MainWindow::cbPrefixes_Change(QString currentIndexText){
	if (currentIndexText.isEmpty())
		return;

	if (tbwGeneral->currentIndex()==2){
		QList<QTreeWidgetItem *> found = twPrograms->findItems(currentIndexText, Qt::MatchWildcard, 0);
		if (found.count()>0){
			twPrograms->setCurrentItem(found.at(0));
			this->twPrograms_ItemClick(found.at(0), 0);
		}
	}
	return;
}

void MainWindow::trayIcon_Activate(QSystemTrayIcon::ActivationReason reason){
	if (reason==QSystemTrayIcon::Trigger){
		if (!isVisible()){
			setMeVisible(TRUE);
		} else {
			setMeVisible(FALSE);
		}
	}
	return;
}

void MainWindow::lstIcons_ItemClick(QString program, QString args, QString desc, QString console, QString desktop){
	/*
	 * This is function for selection icons, and displaying
	 * icon informationm like path and description
	 */

	lblIconInfo0->setText(tr("Program: %1<br> Args: %2").arg(program) .arg(args));
	lblIconInfo2->setText(tr("Description: %1").arg(desc));

	QString useconsole="";
	if (console=="1"){
		useconsole=tr("Yes");
	} else {
		useconsole=tr("No");
	}

	QString desktopsize="";
	if (desktop.isEmpty()){
		desktopsize = tr("Default");
	} else {
		desktopsize = desktop;
	}

	lblIconInfo1->setText(tr("Runs in console: %1<br> Desktop size: %2").arg(useconsole) .arg(desktopsize));

	return;
}

void MainWindow::getSettings(){
	/*
	 * Getting application settings
	 */
	QVariant val;

	val = CoreLib->getSetting("MainWindow", "size", false, QSize(400, 450));
	this->resize(val.toSize());
	val = CoreLib->getSetting("MainWindow", "pos", false, QPoint(200, 200));
	this->move(val.toPoint());

	QList<int> a;
	a.append(CoreLib->getSetting("MainWindow", "splitterSize0", false, 170).toInt());
	a.append(CoreLib->getSetting("MainWindow", "splitterSize1", false, 379).toInt());

	splitter->setSizes(a);

	val = CoreLib->getSetting("wine", "WineBin");
	DEFAULT_WINE_BIN=val.toString();
	val = CoreLib->getSetting("wine", "ServerBin");
	DEFAULT_WINE_SERVER=val.toString();
	val = CoreLib->getSetting("wine", "LoaderBin");
	DEFAULT_WINE_LOADER=val.toString();
	val = CoreLib->getSetting("wine", "WineLibs");
	DEFAULT_WINE_LIBS=val.toString();

	val = CoreLib->getSetting("app", "showTrareyIcon", false);
	SHOW_TRAREY_ICON=val.toBool();

	val = CoreLib->getSetting("system", "tar");
	TAR_BIN=val.toString();
	val = CoreLib->getSetting("system", "mount");
	MOUNT_BIN=val.toString();
	val = CoreLib->getSetting("system", "umount");
	UMOUNT_BIN=val.toString();
	val = CoreLib->getSetting("system", "sudo");
	SUDO_BIN=val.toString();
	val = CoreLib->getSetting("system", "gui_sudo");
	GUI_SUDO_BIN=val.toString();
	val = CoreLib->getSetting("system", "nice");
	NICE_BIN=val.toString();
	val = CoreLib->getSetting("system", "renice");
	RENICE_BIN=val.toString();
	val = CoreLib->getSetting("system", "sh");
	SH_BIN=val.toString();

	val = CoreLib->getSetting("console", "bin");
	CONSOLE_BIN=val.toString();
	val = CoreLib->getSetting("console", "args", false);
	CONSOLE_ARGS=val.toString();

#ifdef WITH_ICOUTILS
	val = CoreLib->getSetting("icotool", "wrestool");
	WRESTOOL_BIN=val.toString();
	val = CoreLib->getSetting("icotool", "icotool");
	ICOTOOL_BIN=val.toString();
#endif



	if (CoreLib->getSetting("quickmount", "type", FALSE).toString().isEmpty()){
		QSettings settings(APP_SHORT_NAME, "default");
		settings.beginGroup("quickmount");

		if (CoreLib->getWhichOut("fuseiso", false).isEmpty()){
#ifdef WITH_EMBEDDED_FUSEISO
			settings.setValue("type", 3);
			settings.setValue("mount_drive_string", CoreLib->getMountString(3));
			settings.setValue("mount_image_string", CoreLib->getMountImageString(3));
			settings.setValue("umount_string", CoreLib->getUmountString(3));
#else
			settings.setValue("type", 0);
			settings.setValue("mount_drive_string", CoreLib->getMountString(0));
			settings.setValue("mount_image_string", CoreLib->getMountImageString(0));
			settings.setValue("umount_string", CoreLib->getUmountString(0));
#endif
		} else {
			settings.setValue("type", 2);
			settings.setValue("mount_drive_string", CoreLib->getMountString(2));
			settings.setValue("mount_image_string", CoreLib->getMountImageString(2));
			settings.setValue("umount_string", CoreLib->getUmountString(2));
		}
		settings.endGroup();
	}

	QString oldDir, oldPrefix;
	oldPrefix = CoreLib->getSetting("LastPrefix", "prefix", false).toString();
	oldDir = CoreLib->getSetting("LastPrefix", "dir", false).toString();

	if (!oldPrefix.isEmpty()){

		cbPrefixes->setCurrentIndex(cbPrefixes->findText(oldPrefix, Qt::MatchExactly));

		for(int i=0; i < twPrograms->topLevelItemCount(); i++)
		{
			std::auto_ptr<QTreeWidgetItem> item(twPrograms->topLevelItem(i));
			if (oldPrefix==item->text(0)){
				if (oldDir.isEmpty()){
					twPrograms->setCurrentItem(item.get());
					twPrograms_ItemClick(item.get(), 0);
					break;
				} else {
					for (int j=0; j < item->childCount(); j++){
						std::auto_ptr<QTreeWidgetItem> childItem(item->child(j));
						if (childItem->text(0)==oldDir){
							twPrograms->setCurrentItem(childItem.get());
							twPrograms_ItemClick(childItem.get(), 0);
						}
						childItem.release();
					}
				}
			}
			item.release();
		}
	}

	/*
  switch (CoreLib->getSetting("network", "type", false).toInt()){
		case 0:
	  proxy.setType(QNetworkProxy::NoProxy);
	  QNetworkProxy::setApplicationProxy(proxy);
	  break;
		case 1:
	  proxy.setType(QNetworkProxy::HttpCachingProxy);
	  proxy.setHostName(CoreLib->getSetting("network", "host", false).toString());
	  proxy.setPort(CoreLib->getSetting("network", "port", false).toInt());
	  proxy.setUser(CoreLib->getSetting("network", "user", false).toString());
	  proxy.setPassword(CoreLib->getSetting("network", "pass", false).toString());
	  proxy.setCapabilities(QNetworkProxy::CachingCapability);
	  QNetworkProxy::setApplicationProxy(proxy);
	  break;
		case 2:
	  proxy.setType(QNetworkProxy::Socks5Proxy);
	  proxy.setHostName(CoreLib->getSetting("network", "host", false).toString());
	  proxy.setPort(CoreLib->getSetting("network", "port", false).toInt());
	  proxy.setUser(CoreLib->getSetting("network", "user", false).toString());
	  proxy.setPassword(CoreLib->getSetting("network", "pass", false).toString());
	  QNetworkProxy::setApplicationProxy(proxy);
	  break;
  }
 */

	return;
}

void MainWindow::changeStatusText(QString text){
	statusBar()->showMessage(text);
}

void MainWindow::updateDtabaseConnectedItems(int currentPrefix){
	/*
		Function for updating objects content to database values
	*/

	QList<QStringList> result, subresult;

	int curRows = 0, numRows = 0;

	// Clearing widgets
	cbPrefixes->clear();
	twPrograms->clear();
	//lstIcons->clear();

	result = db_prefix.getFields();
	for (int i = 0; i < result.size(); ++i) {
		// Inserting root items into programs tree view
		std::auto_ptr<QTreeWidgetItem> prefixItem (new QTreeWidgetItem(twPrograms));
		prefixItem->setText(0, QString("%1").arg(result.at(i).at(1)));
		prefixItem->setIcon(0, loadIcon("data/wine.png"));
		prefixItem->setExpanded (TRUE);
		twPrograms->addTopLevelItem(prefixItem.get());

		// Inserting subfolders items into programs tree view
		subresult = db_dir.getFieldsByPrefixId(result.at(i).at(0));
		for (int j = 0; j < subresult.size(); ++j) {
			std::auto_ptr<QTreeWidgetItem> subPrefixItem (new QTreeWidgetItem(prefixItem.get(), 0));
			subPrefixItem->setText(0, QString("%1").arg(subresult.at(j).at(1)));
			subPrefixItem->setIcon(0, loadIcon("data/folder.png"));
			subPrefixItem.release();
		}

		prefixItem.release();

		// Inserting items into prefixes combo list
		cbPrefixes->addItem (result.at(i).at(1));

		// Inserting items into prefixes table widget
		curRows++;
		numRows = tablePrefix->rowCount();

		if (curRows>numRows){
			tablePrefix->insertRow (numRows);
			numRows = tablePrefix->rowCount();
		}
		if (tablePrefix->item(curRows - 1, 0)){
			tablePrefix->item(curRows - 1, 0)->setText(result.at(i).at(1));
			tablePrefix->item(curRows - 1, 1)->setText(result.at(i).at(2));
		} else {
			std::auto_ptr<QTableWidgetItem> newItem (new QTableWidgetItem(result.at(i).at(1)));
			newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
			tablePrefix->setItem(curRows - 1, 0, newItem.release());
			newItem.reset(new QTableWidgetItem(result.at(i).at(2)));
			newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
			tablePrefix->setItem(curRows - 1, 1, newItem.release());
		}
	}

	numRows = tablePrefix->rowCount();
	if (numRows > curRows){
		for (int i=curRows; i <= numRows; i++)
			tablePrefix->removeRow(curRows);
	}

	if (currentPrefix > 0)
		cbPrefixes->setCurrentIndex (currentPrefix);

	return;
}

void MainWindow::cmdManagePrefixes_Click(){
	tbwGeneral->setCurrentIndex (3);
	return;
}

void MainWindow::tableProc_UpdateContentList(const QModelIndex){
	/*
		Function for updateing tableproc content and QAction status
	*/

	if (tableProc->currentItem()){
		processKillSelected->setEnabled(TRUE);
		processKillWine->setEnabled(TRUE);
		processRenice->setEnabled(TRUE);
	} else {
		processKillSelected->setEnabled(FALSE);
		processKillWine->setEnabled(FALSE);
		processRenice->setEnabled(FALSE);
	}

	getWineProccessInfo();
	return;
}

void MainWindow::tablePrefix_UpdateContentList(const QModelIndex){
	/*
		Function for updateing tablePrefix content and QAction status
	*/

	if (tablePrefix->currentRow()>=0){
		prefixDelete->setEnabled(TRUE);
		prefixImport->setEnabled(TRUE);
		prefixExport->setEnabled(TRUE);
		prefixSettings->setEnabled(TRUE);
	} else {
		prefixDelete->setEnabled(FALSE);
		prefixImport->setEnabled(FALSE);
		prefixExport->setEnabled(FALSE);
		prefixSettings->setEnabled(FALSE);
	}
	return;
}

void MainWindow::tablePrefix_ShowContextMenu(const QPoint){
	menuPrefix->exec(QCursor::pos());
	return;
}

void MainWindow::tableProc_ShowContextMenu(const QPoint point){
	/*
   Function of creation context menu
   */

	if (tableProc->itemAt (point)){
		processKillSelected->setEnabled(TRUE);
		processKillWine->setEnabled(TRUE);
		processRenice->setEnabled(TRUE);
	} else {
		processKillSelected->setEnabled(FALSE);
		processKillWine->setEnabled(FALSE);
		processRenice->setEnabled(FALSE);
	}

	menuProc->exec(QCursor::pos());
	return;

}

void MainWindow::twPrograms_ItemClick(QTreeWidgetItem * item, int){
	/*
	 * This is check for root element, or not.
	 * If yes -- show root-level Icons.
	 * otherwise -- show children-level icons
	 */

	if (!item)
		return;

	if (item->parent()){
		emit(showFolderContents(item->parent()->text(0), item->text(0), txtIconFilter->text()));
	} else {
		emit(showFolderContents(item->text(0), "", txtIconFilter->text()));
	}

	/*
	QList<QStringList> iconsList;

	lstIcons->clear();
	lblIconInfo0->setText(tr("Program:<br> Args:"));
	lblIconInfo1->setText(tr("Runs in console:<br> Desktop size:"));
	lblIconInfo2->setText(tr("Description:"));

	if (!item)
		return;

	if (item->parent()){
		iconsList=db_icon.getByPrefixAndDirName(item->parent()->text(0), item->text(0), txtIconFilter->text());
		cbPrefixes->setCurrentIndex(cbPrefixes->findText(item->parent()->text(0), Qt::MatchExactly));
	} else {
		iconsList=db_icon.getByPrefixAndDirName(item->text(0), "", txtIconFilter->text());
		cbPrefixes->setCurrentIndex(cbPrefixes->findText(item->text(0), Qt::MatchExactly));
	}

	for (int i = 0; i < iconsList.size(); ++i) {
		//QListWidgetItem *iconItem;
		std::auto_ptr<QListWidgetItem> iconItem (new QListWidgetItem(lstIcons.get(), 0));
		iconItem->setText(iconsList.at(i).at(1));

		//Seting icon. If no icon or icon file not exists -- setting default
		if (iconsList.at(i).at(3).isEmpty()){
			iconItem->setIcon(loadIcon("data/exec_wine.png"));
		} else {
			if (QFile::exists (iconsList.at(i).at(3))){
				iconItem->setIcon(QIcon(iconsList.at(i).at(3)));
			} else {
				if (iconsList.at(i).at(3)=="wineconsole"){
					iconItem->setIcon(loadIcon("data/wineconsole.png"));
				} else if (iconsList.at(i).at(3)=="regedit"){
					iconItem->setIcon(loadIcon("data/regedit.png"));
				} else if (iconsList.at(i).at(3)=="wordpad"){
					iconItem->setIcon(loadIcon("data/notepad.png"));
				} else if (iconsList.at(i).at(3)=="winecfg"){
					iconItem->setIcon(loadIcon("data/winecfg.png"));
				} else if (iconsList.at(i).at(3)=="uninstaller"){
					iconItem->setIcon(loadIcon("data/uninstaller.png"));
				} else if (iconsList.at(i).at(3)=="eject"){
					iconItem->setIcon(loadIcon("data/eject.png"));
				} else if (iconsList.at(i).at(3)=="explorer"){
					iconItem->setIcon(loadIcon("data/explorer.png"));
				} else {
					iconItem->setIcon(loadIcon("data/exec_wine.png"));
				}
			}
		}
		iconItem.release();
	}
*/
	return;
}

void MainWindow::twPrograms_ShowContextMenu(const QPoint){
	/*
		Custom context menu for twPrograms
	*/

	/*
	std::auto_ptr<QTreeWidgetItem> treeItem (twPrograms->currentItem());

	QString cdrom_drive="", cdrom_mount="";
	QStringList result;
	QList<QStringList> images;

	if (!treeItem.get())
		return;

	this->twPrograms_ItemClick(treeItem.get(), 0);

	if (treeItem->parent()){
		result = db_prefix.getFieldsByPrefixName(treeItem->parent()->text(0));
	} else {
		result = db_prefix.getFieldsByPrefixName(treeItem->text(0));
	}

	xdgOpenPrefixDir->setEnabled(TRUE);
	winefileOpenPrefixDir->setEnabled(TRUE);

	cdrom_drive = result.at(7);
	cdrom_mount = result.at(6);

	if (cdrom_mount.isEmpty()){
		xdgOpenMountDir->setEnabled(FALSE);
		winefileOpenMountDir->setEnabled(FALSE);
	} else {
		xdgOpenMountDir->setEnabled(TRUE);
		winefileOpenMountDir->setEnabled(TRUE);
	}

	std::auto_ptr<QMenu> menuDirMountImages;

	if (!treeItem->parent()){
		dirRename->setEnabled(FALSE);
		dirDelete->setEnabled(FALSE);
	} else {
		dirRename->setEnabled(TRUE);
		dirDelete->setEnabled(TRUE);
	}

	treeItem.release();

	menuDirMount->clear();
	menuDirMount->setEnabled(FALSE);

	if (!cdrom_mount.isEmpty()){
		menuDirMount->setEnabled(TRUE);
		menuDirMountImages.reset(menuDirMount->addMenu(tr("mount [%1]").arg(CoreLib->getMountedImages(cdrom_mount))));
		if (!cdrom_drive.isEmpty()){
			menuDirMountImages->addAction(QIcon(":/data/drive_menu.png"), cdrom_drive);
			menuDirMountImages->addSeparator();
		} else {
			menuDirMountImages->addAction(QIcon(":/data/drive_menu.png"), tr("[none]"));
			menuDirMountImages->addSeparator();
		}

		images = db_image.getFields();
		for (int i = 0; i < images.size(); ++i) {
			menuDirMountImages->addAction(QIcon(":/data/cdrom_menu.png") , images.at(i).at(0));
		}

		menuDirMount->addSeparator();

		std::auto_ptr<QMenu> menuDirMountRecentImages (menuDirMount->addMenu(tr("mount ...")));
		menuDirMountRecentImages->addAction(iconMountOther.get());
		menuDirMountRecentImages->addSeparator();

		QSettings settings(APP_SHORT_NAME, "default");
		QStringList files = settings.value("recent_images").toStringList();

		for (int i = 0; i < files.size(); ++i){
			menuDirMountRecentImages->addAction(QIcon(":/data/cdrom_menu.png") , files.at(i).split("/").last());
		}

		connect (menuDirMountRecentImages.get(), SIGNAL(triggered(QAction*)), this, SLOT(menuMountRecentImages_triggered(QAction*)));
		menuDirMountRecentImages.release();

		menuDirMount->addSeparator();
		menuDirMount->addAction(dirUnmount.get());

		connect (menuDirMountImages.release(), SIGNAL(triggered(QAction*)), this, SLOT(menuMountImages_triggered(QAction*)));
	}
	if (!cdrom_mount.isEmpty()){
		dirUnmount->setEnabled(TRUE);
		dirMountOther->setEnabled(TRUE);
		dirUnmount->setText(tr("umount [%1]").arg(cdrom_mount));
	} else {
		dirUnmount->setEnabled(FALSE);
		dirMountOther->setEnabled(FALSE);
		dirUnmount->setText(tr("umount [none]"));
	}

	menuRun->addAction(dirRun.get());
	menuRun->addSeparator();
	result = db_last_run_icon.getIcons();

	for (int i=0; i<result.size(); ++i){
		if (i >= recentIconsList.size()){
			std::auto_ptr<QAction> action (new QAction(result.at(i).split("/").last(), this));
			action->setStatusTip(result.at(i));
			recentIconsList.append(action.release());
		} else {
			recentIconsList.at(i)->setText(result.at(i).split("/").last());
			recentIconsList.at(i)->setStatusTip(result.at(i));
		}
		menuRun->addAction(recentIconsList.at(i));
	}

	menuDir->exec(QCursor::pos());
	return;
	*/
}

void MainWindow::menuMountImages_triggered ( QAction * action ){
	/*
	 * This slot process menuDirMountImages and menuIconMountImages triggered signal
	 */

	/*
	if (action->text()==tr("[none]"))
		QMessageBox::warning(this, tr("Error"),  tr("No device drive specified in prefix settings."), QMessageBox::Ok);
	return;

	bool ret;
	if (twPrograms->currentItem()){
		if (twPrograms->currentItem()->parent()){
			ret=CoreLib->mountImage(action->text(), twPrograms->currentItem()->parent()->text(0));
		} else {
			ret=CoreLib->mountImage(action->text(), twPrograms->currentItem()->text(0));
		}
	}

	if (ret){
		statusBar()->showMessage(QObject::tr("%1 successfully mounted.").arg(action->text()));
	} else {
		statusBar()->showMessage(QObject::tr("Fail to mount %1.").arg(action->text()));
	}
	return;
	*/
}


void MainWindow::menuMountRecentImages_triggered ( QAction * action ){
	/*
	 * This slot process menuDirMountImages and menuIconMountImages triggered signal
	 */

	/*
	if (!action)
		return;

	if (action->text().isEmpty())
		return;

	QSettings settings(APP_SHORT_NAME, "default");
	QStringList files = settings.value("recent_images").toStringList();

	for (int i = 0; i < files.size(); ++i){
		if (files.at(i).contains(action->text().split("/").last())){
			bool ret=false;
			if (twPrograms->currentItem()){
				if (twPrograms->currentItem()->parent()){
					ret=CoreLib->mountImage(files.at(i), twPrograms->currentItem()->parent()->text(0));
				} else {
					ret=CoreLib->mountImage(files.at(i), twPrograms->currentItem()->text(0));
				}
			}

			if (ret){
				statusBar()->showMessage(QObject::tr("%1 successfully mounted.").arg(action->text()));
			} else {
				statusBar()->showMessage(QObject::tr("Fail to mount %1.").arg(action->text()));
			}
			break;
		}
	}
*/
	return;
}

void MainWindow::lstIcons_ShowContextMenu(const QPoint & iPoint){
	/*
	*	Function showing context menu
	*/

	/*
	std::auto_ptr<QListWidgetItem> iconItem (lstIcons->itemAt(iPoint));
	std::auto_ptr<QTreeWidgetItem> treeItem (twPrograms->currentItem());

	if (iconItem.get())
		this->lstIcons_ItemClick(iconItem.get());

	QString cdrom_drive="", cdrom_mount="";
	QStringList result;
	QList<QStringList> images;

	if (!treeItem.get())
		return;

	if (treeItem->parent()){
		result = db_prefix.getFieldsByPrefixName(treeItem->parent()->text(0));
	} else {
		result = db_prefix.getFieldsByPrefixName(treeItem->text(0));
	}

	if (result.at(2).isEmpty()){
		xdgOpenPrefixDir->setEnabled(FALSE);
		winefileOpenPrefixDir->setEnabled(FALSE);
	} else {
		xdgOpenPrefixDir->setEnabled(TRUE);
		winefileOpenPrefixDir->setEnabled(TRUE);
	}

	cdrom_drive = result.at(7);
	cdrom_mount = result.at(6);

	if (cdrom_mount.isEmpty()){
		xdgOpenMountDir->setEnabled(FALSE);
		winefileOpenMountDir->setEnabled(FALSE);
	} else {
		xdgOpenMountDir->setEnabled(TRUE);
		winefileOpenMountDir->setEnabled(TRUE);
	}

	menuIconMount->clear();
	menuIconMount->setEnabled(FALSE);

	if (!cdrom_mount.isEmpty()){
		menuIconMount->setEnabled(TRUE);
		std::auto_ptr<QMenu> menuIconMountImages (menuIconMount->addMenu(tr("mount [%1]").arg(CoreLib->getMountedImages(cdrom_mount))));

		if (!cdrom_drive.isEmpty()){
			menuIconMountImages->addAction(QIcon(":/data/drive_menu.png"), cdrom_drive);
			menuIconMountImages->addSeparator();
		} else {
			menuIconMountImages->addAction(QIcon(":/data/drive_menu.png"), tr("[none]"));
			menuIconMountImages->addSeparator();
		}

		menuIconMount->addSeparator();

		std::auto_ptr<QMenu> menuIconMountRecentImages (menuIconMount->addMenu(tr("mount ...")));
		menuIconMountRecentImages->addAction(iconMountOther.get());
		menuIconMountRecentImages->addSeparator();

		QSettings settings(APP_SHORT_NAME, "default");
		QStringList files = settings.value("recent_images").toStringList();

		for (int i = 0; i < files.size(); ++i){
			menuIconMountRecentImages->addAction(QIcon(":/data/cdrom_menu.png") , files.at(i).split("/").last());
		}

		connect (menuIconMountRecentImages.release(), SIGNAL(triggered(QAction*)), this, SLOT(menuMountRecentImages_triggered(QAction*)));

		menuIconMount->addSeparator();
		menuIconMount->addAction(iconUnmount.get());

		images = db_image.getFields();
		for (int i = 0; i < images.size(); ++i) {
			menuIconMountImages->addAction(QIcon(":/data/cdrom_menu.png") , images.at(i).at(0));
		}
		connect (menuIconMountImages.release(), SIGNAL(triggered(QAction*)), this, SLOT(menuMountImages_triggered(QAction*)));

		if ((!cdrom_drive.isEmpty()) && (!cdrom_mount.isEmpty())){
			iconMount->setEnabled(TRUE);
			iconUnmount->setEnabled(TRUE);
			iconMount->setText(tr("mount [%1]").arg(cdrom_drive.split("/").last()));
			iconUnmount->setText(tr("umount [%1]").arg(cdrom_mount));
		} else {
			iconMount->setEnabled(FALSE);
			iconUnmount->setEnabled(FALSE);
			iconMount->setText(tr("mount [none]"));
			iconUnmount->setText(tr("umount [none]"));
		}

		if (!cdrom_drive.isEmpty()){
			iconMountOther->setEnabled(TRUE);
		} else {
			iconMountOther->setEnabled(FALSE);
		}
	}

	if (iconBuffer.names.count()>0){
		iconPaste->setEnabled(TRUE);
	} else {
		iconPaste->setEnabled(FALSE);
	}

	if (lstIcons->selectedItems().count()>0){
		iconDelete->setEnabled(TRUE);
		iconCut->setEnabled(TRUE);
		iconCopy->setEnabled(TRUE);
	} else {
		iconDelete->setEnabled(FALSE);
		iconCut->setEnabled(FALSE);
		iconCopy->setEnabled(FALSE);
	}

	if(iconItem.get()){
		iconRun->setEnabled(TRUE);
		iconOptions->setEnabled(TRUE);
		iconRename->setEnabled(TRUE);
		iconDelete->setEnabled(TRUE);

		if (treeItem->parent()){
			result=db_icon.getByName(treeItem->parent()->text(0), treeItem->text(0), iconItem->text());
		} else {
			result=db_icon.getByName(treeItem->text(0), "", iconItem->text());
		}

		if (!result.at(4).isEmpty()){
			xdgOpenIconDir->setEnabled(TRUE);
			winefileOpenIconDir->setEnabled(TRUE);
		} else {
			xdgOpenIconDir->setEnabled(FALSE);
			winefileOpenIconDir->setEnabled(FALSE);
		}

		menuIcon->exec(QCursor::pos());
	} else {
		xdgOpenIconDir->setEnabled(FALSE);
		winefileOpenIconDir->setEnabled(FALSE);

		menuRun->addAction(dirRun.get());
		menuRun->addSeparator();
		result = db_last_run_icon.getIcons();

		for (int i=0; i<result.size(); ++i){
			if (i >= recentIconsList.size()){
				std::auto_ptr<QAction> action (new QAction(result.at(i).split("/").last(), this));
				action->setStatusTip(result.at(i));
				recentIconsList.append(action.release());
			} else {
				recentIconsList.at(i)->setText(result.at(i).split("/").last());
				recentIconsList.at(i)->setStatusTip(result.at(i));
			}
			menuRun->addAction(recentIconsList.at(i));
		}
		menuIconVoid->exec(QCursor::pos());
	}

	iconItem.release();
	treeItem.release();

	return;
	*/
}

void MainWindow::createTrayIcon(){
	/*
	trayIconMenu.reset(new QMenu(this));
	trayIconMenu->addAction(mainRun);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(mainExportIcons);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(mainImageManage);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(mainPrograms);
	trayIconMenu->addAction(mainProcess);
	trayIconMenu->addAction(mainSetup);
	trayIconMenu->addAction(mainPrefix);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(mainExit);

	trayIcon.reset(new QSystemTrayIcon(this));
	trayIcon->setContextMenu(trayIconMenu.get());

	QIcon icon = loadIcon("data/q4wine.png");

	trayIcon->setIcon(icon);
	setWindowIcon(icon);

	if (SHOW_TRAREY_ICON){
		trayIcon->show();
	} else {
		trayIcon->hide();
	}

	connect(trayIcon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIcon_Activate(QSystemTrayIcon::ActivationReason)));
	*/
}

void MainWindow::closeEvent(QCloseEvent *event){
	/*
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
	}

	std::auto_ptr<QTreeWidgetItem> treeItem (twPrograms->currentItem());

	QSettings settings(APP_SHORT_NAME, "default");
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("splitterSize0", splitter->sizes().at(0));
	settings.setValue("splitterSize1", splitter->sizes().at(1));
	settings.endGroup();
	settings.beginGroup("LastPrefix");
	if (treeItem.get()){
		if (treeItem->parent()){
			settings.setValue("prefix", treeItem->parent()->text(0));
			settings.setValue("dir", treeItem->text(0));
		} else {
			settings.setValue("prefix", treeItem->text(0));
			settings.setValue("dir", "");
		}
	} else {
		settings.setValue("prefix", "");
		settings.setValue("dir", "");
	}

	treeItem.release();
	settings.endGroup();
	*/
	return;
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event){
	// twPrograms events

	/*
	if (obj == this){
		if (event->type()==QEvent::Resize){
			if (!this->isActiveWindow())
				return false;
		}
	}

	if (obj == twPrograms) {
		std::auto_ptr<QKeyEvent> keyEvent (static_cast<QKeyEvent*>(event));
		if (event->type() == QEvent::KeyRelease) {
			if ((keyEvent->key()==Qt::Key_Up) or (keyEvent->key()==Qt::Key_Down)){
				if (twPrograms->currentItem())
					twPrograms_ItemClick(twPrograms->currentItem(), 0);
				keyEvent.release();
				return true;
			}
		}

		if (event->type() == QEvent::KeyPress){
			if ((keyEvent->key()==Qt::Key_Tab)){
				lstIcons->setFocus();
				if (lstIcons->currentItem())
					lstIcons_ItemClick(lstIcons->currentItem());
				keyEvent.release();
				return true;
			}

			if (keyEvent->key()==Qt::Key_Delete){
				if (twPrograms->currentItem())
					dirDelete_Click();
				keyEvent.release();
				return true;
			}

			if (keyEvent->key()==Qt::Key_F2){
				if (twPrograms->currentItem())
					dirRename_Click();
				keyEvent.release();
				return true;
			}
		}
		keyEvent.release();
		return false;

	}

	// lstIcons events

	if (obj == lstIcons.get()){
		std::auto_ptr<QKeyEvent> keyEvent (static_cast<QKeyEvent*>(event));

		if (event->type() == QEvent::KeyRelease) {
			if ((keyEvent->key()==Qt::Key_Up) or (keyEvent->key()==Qt::Key_Down) or (keyEvent->key()==Qt::Key_Left) or (keyEvent->key()==Qt::Key_Right)){
				if (lstIcons->currentItem())
					lstIcons_ItemClick(lstIcons->currentItem());
				keyEvent.release();
				return true;
			}
		}

		if (event->type() == QEvent::KeyPress){
			if ((keyEvent->key()==Qt::Key_Tab)){
				txtIconFilter->setFocus();
				keyEvent.release();
				return true;
			}

			if (keyEvent->key()==Qt::Key_Return){
				if (lstIcons->currentItem())
					lstIcons_ItemDoubleClick(lstIcons->currentItem());
				keyEvent.release();
				return true;
			}

			if (keyEvent->key()==Qt::Key_Delete){
				if (lstIcons->currentItem())
					iconDelete_Click();
				keyEvent.release();
				return true;
			}

			if (keyEvent->key()==Qt::Key_F2){
				if (lstIcons->currentItem())
					iconRename_Click();
				keyEvent.release();
				return true;
			}
		}
		keyEvent.release();
		return false;
	}

	if (obj == txtIconFilter){
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (event->type() == QEvent::KeyPress){
			if ((keyEvent->key()==Qt::Key_Tab)){
				cmdClearFilter->setFocus();
				return true;
			}
		}
		return false;
	}

	if (obj == cmdClearFilter){
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (event->type() == QEvent::KeyPress){
			if ((keyEvent->key()==Qt::Key_Tab)){
				twPrograms->setFocus();
				return true;
			}
		}
		return false;
	}
	*/
	return QMainWindow::eventFilter(obj, event);

}


void MainWindow::resizeEvent (QResizeEvent){
	/*
		Function for hendle resize event: tableProc (см код)
	*/

	CoreFunction_ResizeContent(1);
	CoreFunction_ResizeContent(3);

	return;
}

void MainWindow::CoreFunction_ResizeContent(int tabIndex){
	/*
		Function for hendle resize event

		@screen -- tab id
	*/

	switch (tabIndex){
 case 1:
		//Initiate /proc reading
		if (_IS_TIMER_RUNNING){
			getWineProccessInfo();
			// Starting timer for reading /proc
			timer->start(1000);
		}

		tableProc->resizeRowsToContents();
		tableProc->resizeColumnsToContents();
		tableProc->horizontalHeader()->setStretchLastSection(TRUE);
		break;
		case 3:

		tablePrefix->resizeRowsToContents();
		tablePrefix->resizeColumnsToContents();
		tablePrefix->horizontalHeader()->setStretchLastSection(TRUE);

		// Stopping timer for reading /proc
		if (_IS_TIMER_RUNNING)
			timer->stop();
		break;
		default:
		// Stopping timer for reading /proc
		if (_IS_TIMER_RUNNING)
			timer->stop();
		break;
	}

	return;
}

void MainWindow::processRenice_Click(void){
	/*
	 *	Getting nice level from user
	 */

	bool ok;

	int rowNum;
	rowNum = tableProc->currentRow();

	if (rowNum>=0){
		int curNice;
		curNice = tableProc->item(rowNum, 2)->text().toInt();

		int i = QInputDialog::getInteger(this, tr("Select process priority"), tr("<p>Priority value can be in<br>the range from PRIO_MIN (-20)<br>to PRIO_MAX (20).</p><p>See \"man renice\" for details.</p>"), curNice, -20, 20, 1, &ok);

		if (ok)
			CoreFunction_SetProcNicePriority(i, tableProc->item(rowNum, 0)->text().toInt());
	}
}

void MainWindow::CoreFunction_SetProcNicePriority(int priority, int pid){
	/*
	 * Core function for executing renice command
	 */

	QStringList args;
	QString arg;
	args << RENICE_BIN;
	args.append(tr("%1").arg(priority));
	args.append(tr("%1").arg(pid));

	//Fix args for kdesu\gksu\e.t.c.
	if (!GUI_SUDO_BIN.contains(QRegExp("/sudo$"))){
		arg=args.join(" ");
		args.clear();
		args<<arg;
	}

	Process *exportProcess = new Process(args, GUI_SUDO_BIN, HOME_PATH, tr("reniceing..."), tr("reniceing..."));
	if (exportProcess->exec()==QDialog::Accepted){
		getWineProccessInfo();
	} else {
		statusBar()->showMessage(tr("Renice fail fail"));
	}

	return;
}

void MainWindow::getWineProccessInfo(void){
	// If _RUN_TIMER==FALSE then timer is stopped by user
	if (!_IS_TIMER_RUNNING)
		return;

	QList<QStringList> proclist;
	int numRows = tableProc->rowCount();
	int curRows = 0;

	// Getting QList of QStringList which describes running wine processes
	proclist = CoreLib->getWineProcessList();

	// Preccess QList items one by one
	for (int i = 0; i < proclist.size(); ++i) {
		//If first element value "-1" -- then disable timer and set _IS_TIMER_RUNNING flag
		if (proclist.at(i).at(0) == "-1"){
			_IS_TIMER_RUNNING=false;
			timer->stop();
			return;
		}

		curRows++;

		if (curRows>numRows){
			tableProc->insertRow (numRows);
			numRows = tableProc->rowCount();
		}

		if (tableProc->item(curRows - 1, 0)){
			tableProc->item(curRows - 1, 0)->setText(proclist.at(i).at(0));
			tableProc->item(curRows - 1, 1)->setText(proclist.at(i).at(1));
			tableProc->item(curRows - 1, 2)->setText(proclist.at(i).at(2));
			tableProc->item(curRows - 1, 3)->setText(proclist.at(i).at(3));
		} else {
			QTableWidgetItem *newItem = new QTableWidgetItem(proclist.at(i).at(0));
			tableProc->setItem(curRows - 1, 0, newItem);
			newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
			newItem = new QTableWidgetItem(proclist.at(i).at(1));
			tableProc->setItem(curRows - 1, 1, newItem);
			newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
			newItem = new QTableWidgetItem(proclist.at(i).at(2));
			tableProc->setItem(curRows - 1, 2, newItem);
			newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
			newItem = new QTableWidgetItem(proclist.at(i).at(3));
			tableProc->setItem(curRows - 1, 3, newItem);
			newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
		}

	}


	if (tableProc->currentItem ()){
		processKillSelected->setEnabled(TRUE);
		processKillWine->setEnabled(TRUE);
		processRenice->setEnabled(TRUE);
	} else {
		processKillSelected->setEnabled(FALSE);
		processKillWine->setEnabled(FALSE);
		processRenice->setEnabled(FALSE);
	}


	// Remove unneaded entyes
	numRows = tableProc->rowCount();
	if (numRows > curRows)
		for (int i=curRows; i <= numRows; i++)
			tableProc->removeRow(curRows);


	lblProcInfo->setText(tr("Total process: %1").arg(numRows));

	return;
}

void MainWindow::cmdCreateFake_Click(){

	QString prefixPath = db_prefix.getPath(cbPrefixes->currentText());
	QString sysregPath;
	sysregPath.append(prefixPath);
	sysregPath.append("/system.reg");

	QFile sysreg_file (sysregPath);

	if (sysreg_file.exists()){
		if (QMessageBox::warning(this, tr("Warning"), tr("There are other Fake drive installed in %1 prefix directory.<br><br>Do you wish to REMOVE ALL FILES from prefix folder?").arg(prefixPath), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes){

			QStringList args;
			args << "-rdf";
			args << prefixPath;

			Process *exportProcess = new Process(args, "/bin/rm", HOME_PATH, tr("Removing old fake drive.<br>This can take a while..."), tr("Removing old fake drive"));

			if (exportProcess->exec()!=QDialog::Accepted){
				return;
			}
		} else {
			return;
		}
	}

	QDir fakeDir (prefixPath);
	if (!fakeDir.exists())
		fakeDir.mkdir(prefixPath);

	Wizard *createFakeDriveWizard = new Wizard(2, cbPrefixes->currentText());
	if (createFakeDriveWizard->exec()==QDialog::Accepted){
		updateDtabaseConnectedItems(cbPrefixes->currentIndex());
	}
	delete(createFakeDriveWizard);
	return;
}


void MainWindow::cmdUpdateFake_Click(){
	QString prefixPath = db_prefix.getPath(cbPrefixes->currentText());

	QString sysregPath;
	sysregPath.append(prefixPath);
	sysregPath.append("/system.reg");

	QFile sysreg_file (sysregPath);

	if (!sysreg_file.exists()){
		QMessageBox::warning(this, tr("Error"), tr("Sorry, no fake drive configuration found.<br>Create fake drive configuration before update it!"));
	} else {
		Wizard *createFakeDriveWizard = new Wizard(3, cbPrefixes->currentText());
		if (createFakeDriveWizard->exec()==QDialog::Accepted){
			updateDtabaseConnectedItems(cbPrefixes->currentIndex());
		}
		delete(createFakeDriveWizard);
	}
	return;
}

void MainWindow::cmdClearFilter_Click(){
	txtIconFilter->setText("");
	return;
}

void MainWindow::processKillWine_Click(){
	// Function for killling wineserver for current prefix
	int rowNum;
	rowNum = tableProc->currentRow();

	if (rowNum>=0){
		QString prefixPath;
		prefixPath = tableProc->item(rowNum, 3)->text();

		if (QMessageBox::warning(this, tr("Warning"), tr("This action will send a KILL(-9) signal to all wine proccess with WINEPREFIX='%1'<br><br>Do you really want to proceed?").arg(tableProc->item(rowNum, 3)->text()), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes){
			CoreLib->killWineServer(prefixPath);
		}
	}
	return;
}

void MainWindow::processKillSelected_Click(){
	int rowNum;
	rowNum = tableProc->currentRow();

	if (rowNum>=0){
		QString procId;
		procId = tableProc->item(rowNum, 0)->text();

		if (QMessageBox::warning(this, tr("Warning"), tr("This action will send a KILL(-9) signal to proccess '%2' pid: %1<br><br>It is HIGH risk to damage wine normal state.<br><br>Do you really want to proceed?").arg(tableProc->item(rowNum, 0)->text()) .arg(tableProc->item(rowNum, 1)->text()),                   QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes){

			QString command;
			command="kill -9 ";
			command.append(procId);
			if (system(command.toAscii().data())==-1)
				QMessageBox::warning(this, tr("Error"), tr("Can't run: %1").arg(command.toAscii().data()), QMessageBox::Ok);
		}
	}
	return;
}

/*
 * Prefix commands
 */

void MainWindow::prefixAdd_Click(){
	// Prefix creation function

	Wizard *createPrefixWizard = new Wizard(0);
	if (createPrefixWizard->exec()==QDialog::Accepted){
		updateDtabaseConnectedItems();
	}
	delete(createPrefixWizard);

	return;
}

void MainWindow::prefixDelete_Click(){

	if (tablePrefix->currentRow()>=0){
		if (tablePrefix->item(tablePrefix->currentRow(), 0)->text()=="Default"){
			QMessageBox::warning(this, tr("Error"),
								 tr("Sorry, you can't delete Default prefix."), QMessageBox::Ok);
			return;
		}

		if(QMessageBox::warning(this, tr("Warning"),
								tr("Do you really wish to delete prefix named \"%1\" and all associated icons?").arg(tablePrefix->item(tablePrefix->currentRow(), 0)->text()), QMessageBox::Ok, QMessageBox::Cancel)==QMessageBox::Ok){
			if (db_icon.delIconsByPrefixName(tablePrefix->item(tablePrefix->currentRow(), 0)->text()))
				if(db_dir.delDir(tablePrefix->item(tablePrefix->currentRow(), 0)->text()))
					db_prefix.delByName(tablePrefix->item(tablePrefix->currentRow(), 0)->text());
		}
	}

	updateDtabaseConnectedItems();
	return;
}

void MainWindow::prefixImport_Click(){
	/*
		Function for importing preefix from file
	*/

	QString openpath;

	//Getting user selected path for prefix store, if not -- use default
	if (!PREFIX_EI_PATH.isEmpty()){
		openpath.append(PREFIX_EI_PATH);
	} else {
		openpath.append(QDir::homePath());
		openpath.append("/.config/");
		openpath.append(APP_SHORT_NAME);
		openpath.append("/prefixes/");
	}

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select file to import"), openpath , tr("Prefix archive images (*.tbz)"));

	if (!fileName.isEmpty()){
		QDir dir;
		QString targetDir;

		if (tablePrefix->item(tablePrefix->currentRow(), 1)->text().isEmpty()){
			targetDir.clear();
			targetDir.append(HOME_PATH);
			targetDir.append("/.wine/");
		} else {
			targetDir.clear();
			targetDir.append(tablePrefix->item(tablePrefix->currentRow(), 1)->text());
		}

		if (dir.exists(targetDir)){
			if(QMessageBox::warning(this, tr("Warning"), tr("Do you really wish to delete all old prefix files?"), QMessageBox::Ok, QMessageBox::Cancel)==QMessageBox::Ok){
				QStringList args;
				args << "-rdf";
				args << targetDir;

				Process *exportProcess = new Process(args, CoreLib->getWhichOut("rm"), HOME_PATH, tr("Removing old fake drive.<br>This can take a while..."), tr("Removing old fake drive"));
				if (exportProcess->exec()!=QDialog::Accepted){
					return;
				}
				delete(exportProcess);
			} else {
				return;
			}
		}

		dir.mkdir(targetDir);
		QStringList args;
		args << "-xjf";
		args << fileName;
		args << "-C" << targetDir;

		//Creating process dialog
		Process *exportProcess = new Process(args, TAR_BIN, HOME_PATH, tr("Importing prefix.<br>This can take a while..."), tr("Importing prefix"));
		exportProcess->show();
	}
	return;
}

void MainWindow::prefixSettings_Click(){
	/*
	 *	Getting prefix name, and show settings dialog
	 */
	PrefixSettings settings(tablePrefix->item(tablePrefix->currentRow(), 0)->text());
	if (settings.exec()==QDialog::Accepted){
		updateDtabaseConnectedItems();
	}
	return;
}

void MainWindow::prefixExport_Click(){
	/*
	 * Function for exporting preefix to file
	 */

	QString savepath;

	//Getting user selected path for prefix store, if not -- use default
	if (!PREFIX_EI_PATH.isEmpty()){
		savepath.append(PREFIX_EI_PATH);
	} else {
		savepath.append(QDir::homePath());
		savepath.append("/.config/");
		savepath.append(APP_SHORT_NAME);
		savepath.append("/prefixes/");
	}

	//Creating save path
	savepath.append(tablePrefix->item(tablePrefix->currentRow(), 0)->text());
	savepath.append("-");
	savepath.append(QDate::currentDate().toString(Qt::ISODate));
	savepath.append(".tbz");

	//Request user for valid save path
	QString fileName = QFileDialog::getSaveFileName(this, tr("Select file to export"), savepath , tr("Prefix archive images (*.tbz)"));

	if (!fileName.isEmpty()){
		QStringList args;

		args << "-cjf";
		args << fileName;


		QString prefix_path = tablePrefix->item(tablePrefix->currentRow(), 1)->text();

		if (prefix_path.isEmpty()){
			prefix_path.clear();
			prefix_path.append(HOME_PATH);
			prefix_path.append("/.wine/");
		}

		args << "-C";
		args << prefix_path;
		args << "./";

		//Creating process dialog
		Process *exportProcess = new Process(args, TAR_BIN, prefix_path, tr("Exporting %1 prefix.<br>This can take a while...").arg(tablePrefix->item(tablePrefix->currentRow(), 0)->text()), tr("Exporting prefix"));

		exportProcess->show();
	}

	return;
}

void MainWindow::mainExit_Click(){
	/*
	 * main Menu Exit
	 */

	QSettings settings(APP_SHORT_NAME, "default");
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();

	qApp->quit();
	return;
}

void MainWindow::setMeVisible(bool visible){
	/*
	 * Hide and show MainWindow on TrayIcon click event
	 */
	setVisible(visible);
	return;
}

void MainWindow::mainPrograms_Click(){
	/*
	 * main Menu go Programs manage tool
	 */

	if (!isVisible())
		setMeVisible(TRUE);

	if (isMinimized ())
		showNormal ();


	tbwGeneral->setCurrentIndex (0);
	return;
}

void MainWindow::mainProcess_Click(){
	/*
	 * main Menu go Process manage tool
	 */

	if (!isVisible())
		setMeVisible(TRUE);

	if (isMinimized ())
		showNormal ();

	tbwGeneral->setCurrentIndex ( 1 );
	return;
}

void MainWindow::mainSetup_Click(){
	/*
	 * main Menu go Prefix Setup tool
	 */

	if (!isVisible())
		setMeVisible(TRUE);

	if (isMinimized ())
		showNormal ();

	tbwGeneral->setCurrentIndex ( 2 );
	return;
}

void MainWindow::mainPrefix_Click(){
	/*
	 * main Menu go Prefix manage tool
	 */

	if (!isVisible())
		setMeVisible(TRUE);

	if (isMinimized ())
		showNormal ();

	tbwGeneral->setCurrentIndex ( 3 );
	return;
}

void MainWindow::mainAbout_Click(){
	/*
	 * main Menu shows About dialog
	 */

	if (!isVisible())
		setMeVisible(TRUE);

	if (isMinimized ())
		showNormal ();

	About *about = new About();
	about->exec();
	delete(about);
	return;
}

void MainWindow::mainRun_Click(){
	/*
	 * main Menu shows Run dialog
	 */

	if (!twPrograms->currentItem())
		return;

	QTreeWidgetItem *treeItem = twPrograms->currentItem();
	QStringList dataList;

	if (!isVisible())
		setMeVisible(TRUE);

	if (isMinimized ())
		showNormal ();

	Run run;

	if (treeItem->parent()){
		run.prepare(treeItem->parent()->text(0));
	} else {
		run.prepare(treeItem->text(0));
	}

	if (run.exec()==QDialog::Accepted)
		CoreLib->runWineBinary(run.execObj);

	return;
}


void MainWindow::mainImageManager_Click(){
	/*
	 * CD Image Manager
	 */

	ImageManager *manager = new ImageManager(0);
	manager->exec();

	delete(manager);
	return;
}

void MainWindow::mainOptions_Click(){
	/*
	 * main Menu shows About dialog
	 */

	AppSettings *options = new AppSettings();

	if (options->exec()==QDialog::Accepted){
		getSettings();

		if (SHOW_TRAREY_ICON){
			trayIcon->show();
		} else {
			trayIcon->hide();
		}

	}

	delete(options);
	return;
}

void MainWindow::mainAboutQt_Click(){
	/*
	 * main Menu shows AboutQt dialog
	 */

	QMessageBox::aboutQt ( this );

	return;
}

void MainWindow::mainInstall_Click(){
	/*
	 * main Menu shows install Wizard dialog
	 */

	QMessageBox::warning(this, tr("WIP"), tr("Sorry, no install wizard yet. It'l  implemented at v0.110."));

	return;
}

void MainWindow::mainFirstSteps_Click(){
	CoreLib->openHelpUrl("05-first-steps.html");
}

void MainWindow::mainFAQ_Click(){
	CoreLib->openHelpUrl("00-short-faq.html");
}

void MainWindow::mainIndex_Click(){
	CoreLib->openHelpUrl("index.html");
}

void MainWindow::mainWebsite_Click(){
	CoreLib->openUrl("");
}

void MainWindow::mainDonate_Click(){
	CoreLib->openUrl("donate/");
}

void MainWindow::mainBugs_Click(){
	CoreLib->openUrl("bugs/");
}


void MainWindow::mainExportIcons_Click(){
	/*
	 * main Menu allow export icons
	 */

	if (!isVisible())
		setMeVisible(TRUE);

	QString fileName, tmpDir;
	QStringList args;

	fileName = QFileDialog::getOpenFileName(this, tr("Open image file"), QDir::homePath(), tr("Win32 Executable and Shared libraies (*.exe *.dll);;Win32 Executable (*.exe);;Win32 Shared libraies (*.dll)") );

	if(fileName.isEmpty())
		return;

	args << "-x";
	args << "-t" << "14";

	QStringList list1 = fileName.split("/");

	tmpDir.clear();

	tmpDir.append(QDir::homePath());
	tmpDir.append("/.config/");
	tmpDir.append(APP_SHORT_NAME);
	tmpDir.append("/tmp/");
	tmpDir.append(list1.last());

	QDir tmp(tmpDir);
	tmp.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList list = tmp.entryInfoList();

	if (tmp.exists(tmpDir)){
		for (int i = 0; i < list.size(); ++i) {
			QFileInfo fileInfo = list.at(i);
			if (!tmp.remove(fileInfo.filePath()))
				qDebug()<<"[EE] - Can't delete files at: "<<fileInfo.filePath();
		}

	} else {
		if (!tmp.mkdir(tmpDir)){
			qDebug()<<"[EE] - Can't create temp directory at: "<<tmpDir;
		}
	}


	args << "-o" << tmpDir;
	args << fileName;

	Process *exportProcess = new Process(args, WRESTOOL_BIN, HOME_PATH, tr("Exporting icon from binary file.<br>This can take a while..."), tr("Exporting icon"), FALSE);

	if (exportProcess->exec()==QDialog::Accepted){
		//icotool -x -o ./regedit.png --width=32 --height=32 ./regedit.exe_14_100_0.ico



		args.clear();
		args << "-x";
		args << "-o" << QString("%1/").arg(tmpDir);

		// Updating file index
		list = tmp.entryInfoList();

		//Creating file list for converting
		for (int i = 0; i < list.size(); ++i) {
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.fileName().right(3)=="ico")
				args << fileInfo.filePath();
		}


		//Look here, this function checks is some icons found, or not. 5 -- is default number of arguments,
		//if more -- then we have some ico file to convert
		if (args.size()>=4){

			exportProcess = new Process(args, ICOTOOL_BIN, HOME_PATH, tr("Convering icon from binary file.<br>This can take a while..."), tr("Converting icon"), FALSE);

			if (exportProcess->exec()==QDialog::Accepted){
				IconsView *iconsView = new IconsView(tmpDir);
				iconsView->exec();
			}

		} else {
			IconsView *iconsView = new IconsView(tmpDir);
			iconsView->exec();
		}
	}
	delete(exportProcess);

	//Clearing temp files
	list = tmp.entryInfoList();

	//Creating file list for converting
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		if (!QFile::remove(fileInfo.filePath()))
			qDebug()<<"[EE] - Can't delete files at: "<<fileInfo.filePath();
	}

	if (!tmp.rmdir(tmpDir))
		qDebug()<<"[EE] - Can't delete tmp dir: "<<tmpDir;

	return;
}

void MainWindow::createMenuActions(){
	/*
	 * Context menu for process manage
	 */

	//std::auto_ptr<QMenu> submenuTemplate;

	/*
	processKillSelected.reset(new QAction(loadIcon("data/kill.png"), tr("Stop current"), tableProc));
	processKillSelected->setStatusTip(tr("Send TERM signal to selected process"));
	connect(processKillSelected.get(), SIGNAL(triggered()), this, SLOT(processKillSelected_Click()));

	processKillWine.reset(new QAction(loadIcon("data/stop.png"), tr("Stop wine"), tableProc));
	processKillWine->setStatusTip(tr("Send TERM signal to main wine process"));
	connect(processKillWine.get(), SIGNAL(triggered()), this, SLOT(processKillWine_Click()));

	processRefresh.reset(new QAction(loadIcon("data/reload.png"), tr("Refresh list"), tableProc));
	processRefresh->setStatusTip(tr("Refresh process list"));
	connect(processRefresh.get(), SIGNAL(triggered()), this, SLOT(getWineProccessInfo()));

	processRenice.reset(new QAction(tr("Renice"), tableProc));
	processRenice->setStatusTip(tr("Set process priority"));
	connect(processRenice.get(), SIGNAL(triggered()), this, SLOT(processRenice_Click()));
	*/

	/*
	 * Context menu for directory manage
	 */

	/*
	dirRun.reset(new QAction(QIcon(":/data/folder.png"), tr("Browse ..."), twPrograms));
	dirRun->setStatusTip(tr("Bowse for application"));
	connect(dirRun.get(), SIGNAL(triggered()), this, SLOT(mainRun_Click()));

	dirAdd.reset(new QAction(tr("New"), twPrograms));
	dirAdd->setStatusTip(tr("Create new directory"));
	connect(dirAdd.get(), SIGNAL(triggered()), this, SLOT(dirAdd_Click()));

	dirRename.reset(new QAction(tr("Rename"), twPrograms));
	dirRename->setStatusTip(tr("Rename current directory"));
	connect(dirRename.get(), SIGNAL(triggered()), this, SLOT(dirRename_Click()));

	dirDelete.reset(new QAction(tr("Delete"), twPrograms));
	dirDelete->setStatusTip(tr("Delete current directory"));
	connect(dirDelete.get(), SIGNAL(triggered()), this, SLOT(dirDelete_Click()));

	dirUnmount.reset(new QAction(tr("Unmount cd drive"), twPrograms));
	dirUnmount->setStatusTip(tr("Unmounts cdrom drive"));
	connect(dirUnmount.get(), SIGNAL(triggered()), this, SLOT(dirUnmount_Click()));

	dirMountOther.reset(new QAction(QIcon(":/data/folder.png"), tr("Browse ..."), lstIcons.get()));
	dirMountOther->setStatusTip(tr("Browse for other image"));
	connect(dirMountOther.get(), SIGNAL(triggered()), this, SLOT(dirMountOther_Click()));

	dirConfigure.reset(new QAction(tr("Configure wine"), twPrograms));
	dirConfigure->setStatusTip(tr("Configure Wine general settings"));
	connect(dirConfigure.get(), SIGNAL(triggered()), this, SLOT(dirConfigure_Click()));

	dirInstall.reset(new QAction(tr("App install"), twPrograms));
	dirInstall->setStatusTip(tr("Runs application install wizard for current prefix"));
	connect(dirInstall.get(), SIGNAL(triggered()), this, SLOT(dirInstall_Click()));

	dirUninstall.reset(new QAction(tr("App uninstall"), twPrograms));
	dirUninstall->setStatusTip(tr("Runs application uninstall wizard for current prefix"));
	connect(dirUninstall.get(), SIGNAL(triggered()), this, SLOT(dirUninstall_Click()));
	*/

	/*
	 * Context menus for icon manage
	 */



	/*
	 * Folder open
	 */

	/*
	xdgOpenIconDir.reset(new QAction(tr("Open icon directory"), lstIcons.get()));
	xdgOpenIconDir->setStatusTip(tr("Open directory for current program"));
	connect(xdgOpenIconDir.get(), SIGNAL(triggered()), this, SLOT(xdgOpenIconDir_Click()));

	xdgOpenPrefixDir.reset(new QAction(tr("Open prefix directory"), lstIcons.get()));
	xdgOpenPrefixDir->setStatusTip(tr("Open prefix directory"));
	connect(xdgOpenPrefixDir.get(), SIGNAL(triggered()), this, SLOT(xdgOpenPrefixDir_Click()));

	xdgOpenMountDir.reset(new QAction(tr("Open mount point directory"), lstIcons.get()));
	xdgOpenMountDir->setStatusTip(tr("Open mount point directory for current prefix"));
	connect(xdgOpenMountDir.get(), SIGNAL(triggered()), this, SLOT(xdgOpenMountDir_Click()));

	winefileOpenIconDir.reset(new QAction(tr("Open icon directory"), lstIcons.get()));
	winefileOpenIconDir->setStatusTip(tr("Open directory for current program"));
	connect(winefileOpenIconDir.get(), SIGNAL(triggered()), this, SLOT(winefileOpenIconDir_Click()));

	winefileOpenPrefixDir.reset(new QAction(tr("Open prefix directory"), lstIcons.get()));
	winefileOpenPrefixDir->setStatusTip(tr("Open prefix directory"));
	connect(winefileOpenPrefixDir.get(), SIGNAL(triggered()), this, SLOT(winefileOpenPrefixDir_Click()));

	winefileOpenMountDir.reset(new QAction(tr("Open mount point directory"), lstIcons.get()));
	winefileOpenMountDir->setStatusTip(tr("Open mount point directory for current prefix"));
	connect(winefileOpenMountDir.get(), SIGNAL(triggered()), this, SLOT(winefileOpenMountDir_Click()));
	*/

	/*
	 * Context menus for prefix manage
	 */

	/*
	prefixAdd.reset(new QAction(loadIcon("data/wizard.png"), tr("Create new"), lstIcons.get()));
	prefixAdd->setStatusTip(tr("Create new prefix"));
	connect(prefixAdd.get(), SIGNAL(triggered()), this, SLOT(prefixAdd_Click()));

	prefixImport.reset(new QAction(loadIcon("data/down.png"), tr("Import prefix"), lstIcons.get()));
	prefixImport->setStatusTip(tr("Import prefix"));
	connect(prefixImport.get(), SIGNAL(triggered()), this, SLOT(prefixImport_Click()));
	prefixImport->setEnabled(FALSE);

	prefixExport.reset(new QAction(loadIcon("data/up.png"), tr("Export prefix"), lstIcons.get()));
	prefixExport->setStatusTip(tr("Export prefix"));
	connect(prefixExport.get(), SIGNAL(triggered()), this, SLOT(prefixExport_Click()));
	prefixExport->setEnabled(FALSE);

	prefixDelete.reset(new QAction(loadIcon("data/kill.png"), tr("Delete prefix"), lstIcons.get()));
	prefixDelete->setStatusTip(tr("Delete prefix"));
	connect(prefixDelete.get(), SIGNAL(triggered()), this, SLOT(prefixDelete_Click()));
	prefixDelete->setEnabled(FALSE);

	prefixSettings.reset(new QAction(loadIcon("data/configure.png"), tr("Edit prefix settings"), lstIcons.get()));
	prefixSettings->setStatusTip(tr("Edit prefix settings"));
	connect(prefixSettings.get(), SIGNAL(triggered()), this, SLOT(prefixSettings_Click()));
	prefixSettings->setEnabled(FALSE);

	menuProc.reset(new QMenu(this));
	menuProc->addAction(processKillSelected.get());
	menuProc->addSeparator();
	menuProc->addAction(processKillWine.get());
	menuProc->addSeparator();
	menuProc->addAction(processRenice.get());
	menuProc->addSeparator();
	menuProc->addAction(processRefresh.get());

	menuPrefix.reset(new QMenu(this));
	menuPrefix->addAction(prefixAdd.get());
	menuPrefix->addSeparator();
	menuPrefix->addAction(prefixImport.get());
	menuPrefix->addAction(prefixExport.get());
	menuPrefix->addSeparator();
	menuPrefix->addAction(prefixDelete.get());
	menuPrefix->addSeparator();
	menuPrefix->addAction(prefixSettings.get());

	menuIconMount.reset(new QMenu(tr("Mount iso...")));
	//menuIconMount->addAction(tr("Mount iso..."));


	menuIcon.reset(new QMenu(this));
	menuIcon->addAction(iconRun.get());
	menuIcon->addSeparator();
	menuIcon->addMenu(menuIconMount.get());
	menuIcon->addSeparator();
	menuIcon->addAction(iconOptions.get());
*/
	/*
	submenuTemplate.reset(menuIcon->addMenu(tr("Browser")));
	submenuTemplate->addAction(xdgOpenIconDir.get());
	submenuTemplate->addAction(xdgOpenMountDir.get());
	submenuTemplate->addAction(xdgOpenPrefixDir.get());
	submenuTemplate.release();


	submenuTemplate.reset(menuIcon->addMenu(tr("Wine browser")));
	submenuTemplate->addAction(winefileOpenIconDir.get());
	submenuTemplate->addAction(winefileOpenMountDir.get());
	submenuTemplate->addAction(winefileOpenPrefixDir.get());
	submenuTemplate.release();*/
/*
	menuIcon->addSeparator();
	menuIcon->addAction(iconCut.get());
	menuIcon->addAction(iconCopy.get());
	menuIcon->addAction(iconPaste.get());
	menuIcon->addAction(iconRename.get());
	menuIcon->addAction(iconDelete.get());
	menuIcon->addSeparator();
	menuIcon->addAction(iconAdd.get());

	menuIconVoid.reset(new QMenu(this));
	menuRun .reset(new QMenu(tr("Run..."), this));

	menuIconVoid->addMenu(menuRun.get());
	menuIconVoid->addSeparator();
	menuIconVoid->addAction(iconAdd.get());
	menuIconVoid->addSeparator();
	menuIconVoid->addAction(iconCut.get());
	menuIconVoid->addAction(iconCopy.get());
	menuIconVoid->addAction(iconPaste.get());
	menuIconVoid->addAction(iconDelete.get());
	menuIconVoid->addSeparator();
	menuIconVoid->addMenu(menuIconMount.get());
	menuIconVoid->addSeparator();
*/
	/*
	submenuTemplate.reset(menuIconVoid->addMenu(tr("Browser")));
	submenuTemplate->addAction(xdgOpenMountDir.get());
	submenuTemplate->addAction(xdgOpenPrefixDir.get());
	submenuTemplate.release();

	submenuTemplate.reset(menuIconVoid->addMenu(tr("Wine browser")));
	submenuTemplate->addAction(winefileOpenMountDir.get());
	submenuTemplate->addAction(winefileOpenPrefixDir.get());
	submenuTemplate.release();
	*/
/*
	menuDirMount.reset(new QMenu(tr("Mount iso...")));

	menuDir.reset(new QMenu(this));
	menuDir->addAction(dirAdd.get());
	menuDir->addSeparator();
	menuDir->addMenu(menuDirMount.get());
	menuDir->addSeparator();
	menuDir->addMenu(menuRun.get());
	menuDir->addSeparator();
	menuDir->addAction(dirRename.get());
	menuDir->addSeparator();
	menuDir->addAction(dirDelete.get());
	menuDir->addSeparator();
*/
	/*
	submenuTemplate.reset(menuDir->addMenu(tr("Browser")));
	submenuTemplate->addAction(xdgOpenMountDir.get());
	submenuTemplate->addAction(xdgOpenPrefixDir.get());
	submenuTemplate.release();

	submenuTemplate.reset(menuDir->addMenu(tr("Wine browser")));
	submenuTemplate->addAction(winefileOpenMountDir.get());
	submenuTemplate->addAction(winefileOpenPrefixDir.get());
	submenuTemplate.release();
	*/

	return;
}

QIcon MainWindow::loadIcon(QString iconName){
	// Function tryes to load icon image from theme dir
	// If it fails -> load default from rsource file

	QIcon icon;

	if ((!THEME_NAME.isEmpty()) and (THEME_NAME!="Default")){
		icon.addFile(QString("%1/%2").arg(THEME_NAME).arg(iconName));
		if (icon.isNull()){
			icon.addFile(QString(":/%1").arg(iconName));
		}
	} else {
		icon.addFile(QString(":/%1").arg(iconName));
	}

	return icon;
}

void MainWindow::createToolBarActions(){
	// Toolbar creation function
	/*
	procToolBar.reset(new QToolBar(tlbProccess));
	procToolBar->addAction(processKillSelected.get());
	procToolBar->addAction(processKillWine.get());
	procToolBar->addSeparator ();
	procToolBar->addAction(processRefresh.get());

	prefixToolBar.reset(new QToolBar(tlbPrefix));
	prefixToolBar->addAction(prefixAdd.get());
	prefixToolBar->addSeparator ();
	prefixToolBar->addAction(prefixImport.get());
	prefixToolBar->addAction(prefixExport.get());
	prefixToolBar->addSeparator ();
	prefixToolBar->addAction(prefixSettings.get());
	prefixToolBar->addSeparator ();
	prefixToolBar->addAction(prefixDelete.get());
	*/
	return;
}

/****************************\
 *    CONTEXT MENU SLOTS    *
\****************************/

void MainWindow::iconMountOther_Click(void){
	/*
		This function request mount of selected by user image
	*/

	dirMountOther_Click();

	return;
}

void MainWindow::iconUnmount_Click(void){
	/*
		This function requests unmount by mount point described in icon settings
	*/

	dirUnmount_Click();
	return;
}

void MainWindow::xdgOpenIconDir_Click(void){
	/*
	QTreeWidgetItem *treeItem = twPrograms->currentItem();
	QListWidgetItem *iconItem = lstIcons->currentItem();
	QStringList result;
	if (!treeItem)
		return;
	if (!iconItem)
		return;

	if (treeItem->parent()){
		result = db_icon.getByName(treeItem->parent()->text(0), treeItem->text(0), iconItem->text());
	} else {
		result = db_icon.getByName(treeItem->text(0), "", iconItem->text());
	}

	QStringList args;
	args<<result.at(4);
	CoreLib->runProcess(CoreLib->getWhichOut("xdg-open"), args, "", FALSE);
*/
	return;
}

void MainWindow::xdgOpenUrl(QString url){
	qDebug()<<"FUCK"<<url;
	QStringList args;
	args<<url;
	CoreLib->runProcess(CoreLib->getWhichOut("xdg-open"), args, "", FALSE);
	return;
}

void MainWindow::xdgOpenPrefixDir_Click(void){
	QTreeWidgetItem *treeItem = twPrograms->currentItem();

	QString result;
	if (!treeItem)
		return;

	if (treeItem->parent()){
		result = db_prefix.getPath(treeItem->parent()->text(0));
	} else {
		result = db_prefix.getPath(treeItem->text(0));
	}

	QStringList args;
	args<<result;
	CoreLib->runProcess(CoreLib->getWhichOut("xdg-open"), args, "", FALSE);

	return;
}


void MainWindow::xdgOpenMountDir_Click(void){
	QTreeWidgetItem *treeItem = twPrograms->currentItem();

	QString result;
	if (!treeItem)
		return;

	if (treeItem->parent()){
		result = db_prefix.getMountPath(treeItem->parent()->text(0));
	} else {
		result = db_prefix.getMountPath(treeItem->text(0));
	}

	QStringList args;
	args<<result;
	CoreLib->runProcess(CoreLib->getWhichOut("xdg-open"), args, "", FALSE);
}



void MainWindow::winefileOpenIconDir_Click(void){
	/*
	QTreeWidgetItem *treeItem = twPrograms->currentItem();
	QListWidgetItem *iconItem = lstIcons->currentItem();
	QStringList result;
	if (!treeItem)
		return;
	if (!iconItem)
		return;

	if (treeItem->parent()){
		result = db_icon.getByName(treeItem->parent()->text(0), treeItem->text(0), iconItem->text());
		CoreLib->runWineBinary("winefile", result.at(4) + "/", treeItem->parent()->text(0));
	} else {
		result = db_icon.getByName(treeItem->text(0), "", iconItem->text());
		CoreLib->runWineBinary("winefile", result.at(4) + "/", treeItem->text(0));
	}
*/
	return;
}

void MainWindow::winefileOpenPrefixDir_Click(void){
	QTreeWidgetItem *treeItem = twPrograms->currentItem();

	QString result;
	if (!treeItem)
		return;

	if (treeItem->parent()){
		result = db_prefix.getPath(treeItem->parent()->text(0));
		CoreLib->runWineBinary("winefile", result + "/", treeItem->parent()->text(0));
	} else {
		result = db_prefix.getPath(treeItem->text(0));
		CoreLib->runWineBinary("winefile", result + "/", treeItem->text(0));
	}

	return;
}


void MainWindow::winefileOpenMountDir_Click(void){
	QTreeWidgetItem *treeItem = twPrograms->currentItem();

	QString result;
	if (!treeItem)
		return;

	if (treeItem->parent()){
		result = db_prefix.getMountPath(treeItem->parent()->text(0));
		CoreLib->runWineBinary("winefile", result + "/", treeItem->parent()->text(0));
	} else {
		result = db_prefix.getMountPath(treeItem->text(0));
		CoreLib->runWineBinary("winefile", result + "/", treeItem->text(0));
	}

}


void MainWindow::dirAdd_Click(void){
	QTreeWidgetItem *treeItem = twPrograms->currentItem();
	QTreeWidgetItem *prefixItem;

	if (!treeItem->text(0).isEmpty()){
		bool ok;
		QString dirname = QInputDialog::getText(this, tr("Enter new directory name"), tr("Directory name:"), QLineEdit::Normal, "" , &ok);

		if (ok && !dirname.isEmpty()){

			if (treeItem->parent()){
				if (db_dir.isExistsByName(treeItem->parent()->text(0), dirname)){
					QMessageBox::warning(this, tr("Error"), tr("Sorry, but directory named %1 already exists.").arg(dirname));
					return;
				}

				if (!db_dir.addDir(treeItem->parent()->text(0), dirname))
					return;
				prefixItem = new QTreeWidgetItem(treeItem->parent());
			} else {
				if (db_dir.isExistsByName(treeItem->text(0), dirname)){
					QMessageBox::warning(this, tr("Error"), tr("Sorry, but directory named %1 already exists.").arg(dirname));
					return;
				}

				if (!db_dir.addDir(treeItem->text(0), dirname))
					return;
				prefixItem = new QTreeWidgetItem(treeItem);
			}

			prefixItem->setText(0, dirname);
			prefixItem->setIcon(0, loadIcon("data/folder.png"));
		}
	}
	return;
}

void MainWindow::dirUnmount_Click(void){
	/*
		Request for unmounting cdrom drve described at wine prefix settings
	*/

	bool ret=false;
	if (twPrograms->currentItem()){
		if (twPrograms->currentItem()->parent()){
			ret=CoreLib->umountImage(twPrograms->currentItem()->parent()->text(0));
		} else {
			ret=CoreLib->umountImage(twPrograms->currentItem()->text(0));
		}
	}

	if (ret){
		statusBar()->showMessage(QObject::tr("Drive successfully umounted."));
	} else {
		statusBar()->showMessage(QObject::tr("Fail to umount drive."));
	}
	return;
}

void MainWindow::dirMountOther_Click(void){
	/*
	Request for unmounting cdrom drve described at wine prefix settings
	*/
#ifdef _OS_LINUX_
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open CD Image files"), HOME_PATH, tr("CD image files (*.iso *.nrg *.img *.bin *.mdf)"));
#endif

#ifdef _OS_FREEBSD_
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open ISO Image file"), HOME_PATH, tr("iso files (*.iso)"));
#endif

	if(fileName.isEmpty()){
		return;
	}

	bool ret=false;
	if (twPrograms->currentItem()){
		if (twPrograms->currentItem()->parent()){
			ret=CoreLib->mountImage(fileName, twPrograms->currentItem()->parent()->text(0));
		} else {
			ret=CoreLib->mountImage(fileName, twPrograms->currentItem()->text(0));
		}
	}

	if (ret){
		statusBar()->showMessage(QObject::tr("%1 successfully mounted.").arg(fileName));
	} else {
		statusBar()->showMessage(QObject::tr("Fail to mount %1.").arg(fileName));
	}

	QSettings settings(APP_SHORT_NAME, "default");
	QStringList files = settings.value("recent_images").toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > 8)
		files.removeLast();

	settings.setValue("recent_images", files);

	return;
}

void MainWindow::dirConfigure_Click(void){
	//FIXME: What is this? ;)
	//RunWineUtils("winecfg", twPrograms->currentItem());
	return;
}

void MainWindow::dirInstall_Click(void){
	//FIXME: no install wizard yet...
	//RunWineUtils("winecfg", twPrograms->currentItem());

	QMessageBox::warning(this, tr("WIP"), tr("Sorry, no install wizard yet. It'l implemented at v0.110."));

	return;
}

void MainWindow::dirUninstall_Click(void){
	//FIXME: Переделать на новый класс winebinlauncher
	//RunWineUtils("uninstaller", twPrograms->currentItem());
	return;
}

void MainWindow::dirRename_Click(void){
	QTreeWidgetItem *treeItem = twPrograms->currentItem();

	if (!treeItem)
		return;

	if (treeItem->parent()){
		bool ok = FALSE;
		QString newName = QInputDialog::getText(this, tr("Enter new name for directory"), tr("Directory name:"), QLineEdit::Normal, treeItem->text(0) , &ok);

		if (ok && !newName.isEmpty()){
			while (db_dir.isExistsByName(treeItem->parent()->text(0), newName)){
				newName = QInputDialog::getText(this, tr("Sorry. It seems directory already exists."), tr("Sorry. It seems directory already exists.<br>Please choose another name, or cancel operation."), QLineEdit::Normal, newName, &ok);
				if ((!ok) || (newName.isEmpty())){
					return;
				}
			}
			db_dir.renameDir(treeItem->text(0), treeItem->parent()->text(0), newName);
			treeItem->setText(0, newName);
		}

	}
	return;
}

void MainWindow::dirDelete_Click(void){
	QTreeWidgetItem *treeItem;
	treeItem = twPrograms->currentItem();

	if (!treeItem)
		return;

	if (treeItem->parent()){

		if (QMessageBox::warning(this, tr("Q4Wine"), tr("Do you really wish delete folder named \"%1\" and all associated icons?\n").arg(twPrograms->currentItem()->text(0)),
								 QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes){

			if (db_icon.delIcon(treeItem->parent()->text(0), treeItem->text(0), ""))
				if(db_dir.delDir(treeItem->parent()->text(0), treeItem->text(0)))
					treeItem->parent()->removeChild (treeItem);
			twPrograms_ItemClick(twPrograms->currentItem(), 0);
		}
	}

	return;
}

void MainWindow::runAutostart(void){
	QList<QStringList> iconsList, prefixList;

	prefixList = db_prefix.getFields();
	for (int i = 0; i < prefixList.size(); ++i) {
		iconsList = db_icon.getByPrefixAndDirName(prefixList.at(i).at(1), "autostart");
		for (int j = 0; j < iconsList.size(); ++j) {
			CoreLib->runIcon(prefixList.at(i).at(1), "autostart", iconsList.at(j).at(1));
		}
	}
	return;
}

void MainWindow::messageReceived(const QString message) const{
	statusBar()->showMessage(message);
	return;
}
