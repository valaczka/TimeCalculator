import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

QPage {
	id: root

	property var editData: null

	title: qsTr("Importálás")
	subtitle: editData ? editData.name : ""

	QScrollable {
		id: _scrollable

		anchors.fill: parent
		contentCentered: true
		refreshEnabled: false

		Column {
			spacing: 20

			width: parent.width

			QLabelInformative {
				text: qsTr("(1) Tölts le egy üres sablont")
			}


			Qaterial.OutlineButton {
				anchors.horizontalCenter: parent.horizontalCenter
				highlighted: false
				text: qsTr("Sablon letöltése")
				icon.source: Qaterial.Icons.fileDocumentOutline
				onClicked: {
					App.importTemplateDownload()
				}
			}


			QLabelInformative {
				text: qsTr("(2) Töltsd fel az importálandó táblázatot")
			}

			QButton {
				anchors.horizontalCenter: parent.horizontalCenter
				highlighted: true
				text: qsTr("Feltöltés")
				icon.source: Qaterial.Icons.upload
				onClicked: {
					App.import()
				}
			}
		}
	}
}
