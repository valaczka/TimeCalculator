import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial
import "JScript.js" as JS

QPage {
	id: control

	title: qsTr("Gyakorlati idő kalkulátor")
	subtitle: App.version

	appBar.backButtonVisible: false
	appBar.rightComponent: Qaterial.AppBarButton
	{
		icon.source: Qaterial.Icons.dotsVertical
		onClicked: menu.open()

		QMenu {
			id: menu

			QMenuItem { action: actionAbout }
		}
	}


	Column {
		anchors.centerIn: parent

		QButton {
			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("Létrehozás")
			icon.source: Qaterial.Icons.plus
			enabled: !App.database
			onClicked: {
				Qaterial.DialogManager.showTextFieldDialog({
															   textTitle: qsTr("Munkavállaló neve"),
															   title: qsTr("Új adatbázis létrehozása"),
															   standardButtons: DialogButtonBox.Cancel | DialogButtonBox.Ok,
															   onAccepted: function(_text, _noerror) {
																   App.dbCreate(_text)
															   }
														   })

			}
		}

		QButton {
			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("Megnyitás")
			icon.source: Qaterial.Icons.folderOpen
			enabled: !App.database
			onClicked: {
				App.dbOpen()
			}
		}
	}


	Action {
		id: actionAbout
		text: qsTr("Névjegy")
		icon.source: Qaterial.Icons.information
		onTriggered: {
			Qaterial.DialogManager.showDialog(
						{
							text: qsTr("Verzió: %1\n(c) 2023 Valaczka János Pál").arg(App.version),
							title: control.title,
							iconSource: "qrc:/piar.png",
							iconColor: "transparent",
							iconFill: false,
							iconSize: Qaterial.Style.roundIcon.size,
							standardButtons: DialogButtonBox.Close
						})
		}
	}

}
