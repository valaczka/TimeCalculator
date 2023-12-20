import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

QPage {
	id: root

	property var editData: null
	property int _calcDestination: 1

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

			width: Math.min(parent.width, Qaterial.Style.maxContainerSize, Qaterial.Style.pixelSizeRatio*600)

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
				text: qsTr("Jelenlegi jogviszony (piarista)")
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
				to: editData ? (_jobYears.value == _jobYears.to ? editData.durationDays : 364) : 0
				spin.editable: true
				visible: _job.currentValue === 2
			}

			QButton {
				anchors.horizontalCenter: parent.horizontalCenter
				highlighted: true
				icon.source: Qaterial.Icons.calculator
				text: qsTr("Számítás")
				visible: _job.currentValue === 2 && _rptr.count
				onClicked: loadCalculator(1)
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

			QButton {
				anchors.horizontalCenter: parent.horizontalCenter
				highlighted: true
				icon.source: Qaterial.Icons.calculator
				text: qsTr("Számítás")
				visible: _practice.currentValue === 2 && _rptr.count
				onClicked: loadCalculator(2)
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
				to: editData ? (_prestigeYears.value == _prestigeYears.to ? editData.durationDays : 364) : 0
				spin.editable: true
				visible: _prestige.currentValue === 2
			}

			QButton {
				anchors.horizontalCenter: parent.horizontalCenter
				highlighted: true
				icon.source: Qaterial.Icons.calculator
				text: qsTr("Számítás")
				visible: _prestige.currentValue === 2 && _rptr.count
				onClicked: loadCalculator(3)
			}

			QButton
			{
				anchors.horizontalCenter: parent.horizontalCenter
				action: _actionSave
			}


			QFormSection {
				icon.source: Qaterial.Icons.calendarCollapseHorizontal
				text: qsTr("Átfedések")
				color: Qaterial.Style.iconColor()
				visible: _rptr.count
			}

			Repeater {
				id: _rptr

				delegate: Qaterial.ItemDelegate {
					width: _form.width
					text: modelData.name
					secondaryText: new Date(modelData.start).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")
								   + (modelData.end ? (" - " + new Date(modelData.end).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")) : "")
				}
			}


		}



	}



	function loadCalculator(_dest) {
		_calcDestination = _dest
		Qaterial.DialogManager.openFromComponent(_dialog)
	}




	Component {
		id: _dialog

		Qaterial.ModalDialog
		{
			id: control

			dialogImplicitWidth: 600

			title: switch(root._calcDestination) {
				   case 1: return qsTr("Jelenlegi jogviszony (piarista)")
				   case 2: return qsTr("Szakmai gyakorlat")
				   default: return qsTr("Jubileumi jutalom")
				   }


			contentItem: GridLayout
			{
				id: _grid
				rowSpacing: 10
				columnSpacing: 5
				columns: 2

				Qaterial.LabelCaption {
					text: qsTr("Első nap:")
				}

				Qaterial.ComboBox {
					id: _combo1
					font: Qaterial.Style.textTheme.body2
					model: _calcModel
					textRole: "text"
					valueRole: "date"
					Layout.fillWidth: true
					onActivated: _grid.recalc()
				}

				Qaterial.LabelCaption {
					text: qsTr("Utolsó nap:")
				}

				Qaterial.ComboBox {
					id: _combo2
					font: Qaterial.Style.textTheme.body2
					model: _calcModel
					textRole: "text"
					valueRole: "date"
					Layout.fillWidth: true
					onActivated: _grid.recalc()
				}

				Qaterial.LabelCaption {
					text: qsTr("Időtartam:")
				}

				Qaterial.LabelBody1 {
					id: _lblCalc
					property int years: 0
					property int days: 0
					text: qsTr("%1 év %2 nap").arg(years).arg(days)
				}

				function recalc() {
					_lblCalc.years = App.yearsBetween(_combo1.currentValue, _combo2.currentValue)
					_lblCalc.days = App.daysBetween(_combo1.currentValue, _combo2.currentValue)
				}
			}

			standardButtons: Dialog.Apply | Dialog.Close

			onApplied: {
				switch(root._calcDestination) {
				case 1:
					_jobYears.value = _lblCalc.years
					_jobDays.value = _lblCalc.days
					break
				case 2:
					_practiceYears.value = _lblCalc.years
					_practiceDays.value = _lblCalc.days
					break
				case 3:
					_prestigeYears.value = _lblCalc.years
					_prestigeDays.value = _lblCalc.days
					break
				}
				_form.modified = true
				close()
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


	ListModel {
		id: _calcModel
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


		let list = App.database.overlapGet(editData.id)

		_rptr.model = list

		_calcModel.clear()

		_calcModel.append({
							  date: editData.start,
							  text: editData.name+qsTr(" kezdete: ")+new Date(editData.start).toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
						  })

		if (editData.end) {
			_calcModel.append({
								  date: editData.end,
								  text: editData.name+qsTr(" vége: ")+new Date(editData.end).toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
							  })
		}

		for (let i=0; i<list.length; ++i) {
			let ll = list[i]

			_calcModel.append({
								  date: ll.start,
								  text: ll.name+qsTr(" kezdete: ")+new Date(ll.start).toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
							  })

			if (ll.end) {
				_calcModel.append({
									  date: ll.end,
									  text: ll.name+qsTr(" vége: ")+new Date(ll.end).toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
								  })
			}
		}

	}

}
