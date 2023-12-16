import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

QPage {
	id: control

	stackPopFunction: function() {
		/*if (view.selectEnabled) {
			view.unselectAll()
			return false
		}*/
		return true
	}

	title: "Test-SQL"

	appBar.backButtonVisible: false
	appBar.rightComponent: Qaterial.AppBarButton
	{
		icon.source: Qaterial.Icons.dotsVertical
		/*onClicked: (Qt.platform.os === "windows" || Qt.platform.os === "linux") ? menuDesktop.open() : menu.open()

		QMenu {
			id: menu

			QMenuItem { action: actionAdd }
			QMenuItem { action: actionQR }
			Qaterial.MenuSeparator {}
			QMenuItem { action: actionDemo }
			Qaterial.MenuSeparator {}
			QMenuItem { action: actionSettings }
			QMenuItem { action: actionAbout }
			QMenuItem { action: actionExit }
		}

		QMenu {
			id: menuDesktop

			QMenuItem { action: actionAdd }
			QMenuItem { action: actionQR }
			Qaterial.MenuSeparator {}
			QMenuItem { action: actionDemo }
			QMenuItem { action: actionEditor }
			Qaterial.MenuSeparator {}
			QMenuItem { action: actionSettings }
			QMenuItem { action: actionAbout }
			QMenuItem { action: actionExit }
		}*/
 }

/*
	Action {
		id: actionAbout
		text: qsTr("Névjegy")
		icon.source: "qrc:/internal/img/callofsuli_square.svg"
		onTriggered: {
			onClicked: Client.stackPushPage("PageAbout.qml", {})
		}
	}

	Action {
		id: actionExit
		icon.source: Qaterial.Icons.applicationExport
		text: qsTr("Kilépés")
		onTriggered: Client.closeWindow()
	}
*/




	Qaterial.Banner
	{
		anchors.top: parent.top
		width: parent.width
		drawSeparator: true
		text: qsTr("Árvíztűrő tükörfúrógép ŰŰÉÁŰA")
		iconSource: Qaterial.Icons.desktopClassic
		fillIcon: false
		outlinedIcon: true
		highlightedIcon: true

		action1: qsTr("QR-kód")
		action2: qsTr("Hozzáadás")

		//onAction1Clicked: actionQR.trigger()
		onAction2Clicked: App.testSql()

		//enabled: !Client.serverList.length
		//visible: !Client.serverList.length
	}


	Qaterial.LabelHeadline3 {
		id: _label
		font.italic: true

		anchors.centerIn: parent
	}

	Connections {
		target: App

		function onAddedToDb(txt) {
			_label.text = txt
		}
	}
}
