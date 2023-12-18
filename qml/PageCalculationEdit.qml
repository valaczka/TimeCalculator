import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

QPage {
	id: root

	property var editData: null

	closeQuestion: _form.modified ? qsTr("Biztosan eldobod a módosításokat?") : ""

	title: editData ? editData.name : ""
	subtitle: editData ? new Date(editData.start).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")
						 + (editData.end ? (" - " + new Date(editData.end).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")) : "")
						 + qsTr(" (%1 év %2 nap)").arg(editData.durationYears).arg(editData.durationDays)
					   : ""

	appBar.rightComponent: Qaterial.AppBarButton
	{
		action: _actionSave
		display: AbstractButton.IconOnly

	}

	ListModel {
		id: _comboModel
		ListElement { value: 0; text: qsTr("nem") }
		ListElement { value: 1; text: qsTr("igen") }
		ListElement { value: 2; text: qsTr("részben") }
	}


	QScrollable {
		id: _scrollable

		anchors.fill: parent

		QFormColumn {
			id: _form

			width: Math.min(parent.width, Qaterial.Style.maxContainerSize, Qaterial.Style.pixelSizeRatio*500)

			Qaterial.LabelBody1 {
				width: parent.width
				wrapMode: Label.Wrap
				text: qsTr("Foglalkoztatási jogviszony típusa: %1").arg(editData ? editData.type : "?")
			}

			Qaterial.LabelBody1 {
				width: parent.width
				wrapMode: Label.Wrap
				text: qsTr("Heti munkaidő: %1 óra").arg(editData ? editData.hour : "?")
			}

			Qaterial.LabelBody1 {
				width: parent.width
				wrapMode: Label.Wrap
				text: qsTr("Heti óraszámy: %1 óra").arg(editData ? editData.value : "?")
				bottomPadding: 30
			}

			QFormSection {
				icon.source: Qaterial.Icons.briefcaseVariant
				text: qsTr("Munkaviszony")
			}

			QFormComboBox {
				id: _job
				field: "jobMode"
				text: qsTr("Beszámítás:")
				model: _comboModel
				combo.implicitWidth: Qaterial.Style.pixelSizeRatio*300
				textRole: "text"
				valueRole: "value"
			}

			QFormSpinBox {
				id: _jobYears
				field: "jobYears"
				text: qsTr("Számított évek száma:")
				from: 0
				to: editData ? editData.durationYears : 0
				spin.editable: true
				visible: _job.currentValue === 2
			}

			QFormSpinBox {
				id: _jobDays
				field: "jobDays"
				text: qsTr("Számított napok száma:")
				from: 0
				to: editData ? (_jobYears.value == _jobYears.to ? editData.durationDays : 365) : 0
				spin.editable: true
				visible: _job.currentValue === 2
			}



			QFormSection {
				icon.source: Qaterial.Icons.hammerWrench
				text: qsTr("Szakmai gyakorlat")
			}

			QFormComboBox {
				id: _practice
				field: "practiceMode"
				text: qsTr("Beszámítás:")
				model: _comboModel
				combo.implicitWidth: Qaterial.Style.pixelSizeRatio*300
				textRole: "text"
				valueRole: "value"
			}

			QFormSpinBox {
				id: _practiceYears
				field: "practiceYears"
				text: qsTr("Számított évek száma:")
				from: 0
				to: editData ? editData.durationYears : 0
				spin.editable: true
				visible: _practice.currentValue === 2
			}

			QFormSpinBox {
				id: _practiceDays
				field: "practiceDays"
				text: qsTr("Számított napok száma:")
				from: 0
				to: editData ? (_practiceYears.value == _practiceYears.to ? editData.durationDays : 364) : 0
				spin.editable: true
				visible: _practice.currentValue === 2
			}


			QFormSection {
				icon.source: Qaterial.Icons.medal
				text: qsTr("Jubileumi jutalom")
			}

			QFormComboBox {
				id: _prestige
				field: "prestigeMode"
				text: qsTr("Beszámítás:")
				model: _comboModel
				combo.implicitWidth: Qaterial.Style.pixelSizeRatio*300
				textRole: "text"
				valueRole: "value"
			}

			QFormSpinBox {
				id: _prestigeYears
				field: "prestigeYears"
				text: qsTr("Számított évek száma:")
				from: 0
				to: editData ? editData.durationYears : 0
				spin.editable: true
				visible: _prestige.currentValue === 2
			}

			QFormSpinBox {
				id: _prestigeDays
				field: "prestigeDays"
				text: qsTr("Számított napok száma:")
				from: 0
				to: editData ? (_prestigeYears.value == _prestigeYears.to ? editData.durationDays : 365) : 0
				spin.editable: true
				visible: _prestige.currentValue === 2
			}

			QButton
			{
				anchors.horizontalCenter: parent.horizontalCenter
				action: _actionSave
			}
		}

	}




	Action {
		id: _actionSave
		text: qsTr("Kész")
		icon.source: Qaterial.Icons.checkBold
		enabled: _form.modified
		onTriggered:
		{
			let d = _form.getItems([
									   _job, _jobYears, _jobDays,
									   _practice, _practiceYears, _practiceDays,
									   _prestige, _prestigeYears, _prestigeDays,
								   ])

			if (App.database.calculationEdit(editData.id, d)) {
				App.snack(qsTr("Sikeres módosítás"))
				_form.modified = false
				App.stackPop()
			} else {
				App.messageError(qsTr("Sikertelen módosítás"))
			}
		}
	}



	Component.onCompleted: {
		if (!editData)
			return

		let d = App.database.calculationGet(editData.id)

		_form.setItems([
						   _job, _jobYears, _jobDays,
						   _practice, _practiceYears, _practiceDays,
						   _prestige, _prestigeYears, _prestigeDays,
					   ], d)

	}

}
