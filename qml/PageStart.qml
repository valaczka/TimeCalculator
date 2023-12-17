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
	}


	Column {
		anchors.centerIn: parent

		QButton {
			text: "New"
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
			text: "Open"
			enabled: !App.database
			onClicked: {
				App.dbOpen()
			}
		}

		QButton {
			text: "Save"
			enabled: App.database
			onClicked: App.dbSave()
		}
	}


}
