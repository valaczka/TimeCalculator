import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

QPage {
	id: root

	property var editData: null

	closeQuestion: _form.modified ? qsTr("Biztosan eldobod a módosításokat?") : ""

	title: editData ? editData.name : qsTr("Új munkakör")


	appBar.rightComponent: Row {
		Qaterial.AppBarButton
		{
			action: _actionDelete
			anchors.verticalCenter: parent.verticalCenter
			display: AbstractButton.IconOnly
			visible: editData
		}
		Qaterial.AppBarButton
		{
			action: _actionSave
			anchors.verticalCenter: parent.verticalCenter
			display: AbstractButton.IconOnly
		}
	}


	QScrollable {
		id: _scrollable

		anchors.fill: parent

		QFormColumn {
			id: _form

			title: qsTr("Jogviszony adatai")

			QFormTextField {
				id: _start
				field: "start"
				title: qsTr("Jogviszony kezdete")
				width: parent.width
				helperText: qsTr("A jogviszony első napja")
				placeholderText: qsTr("ÉÉÉÉ-HH-NN")
				validator: RegularExpressionValidator {
					regularExpression: /(^\d{4}\-(0[1-9]|1[012])\-(0[1-9]|[12][0-9]|3[01])$)/
				}
				errorText: qsTr("Adj meg egy érvényes dátumot ebben a formában: ÉÉÉÉ-HH-NN")
				leadingIconSource: Qaterial.Icons.calendarStart
				trailingContent: Qaterial.TextFieldButtonContainer
				{
					Qaterial.TextFieldAlertIcon {  }
					Qaterial.TextFieldClearButton { }
				}

				onEditingFinished: recalcDuration()
			}

			QFormTextField {
				id: _end
				field: "end"
				title: qsTr("Jogviszony vége")
				width: parent.width
				helperText: qsTr("A jogviszony utolsó napja (ha már nem tart)")
				placeholderText: qsTr("ÉÉÉÉ-HH-NN")
				validator: RegularExpressionValidator {
					regularExpression: /(^\d{4}\-(0[1-9]|1[012])\-(0[1-9]|[12][0-9]|3[01])$)|^$/
				}
				errorText: qsTr("Adj meg egy érvényes dátumot ebben a formában: ÉÉÉÉ-HH-NN")
				leadingIconSource: Qaterial.Icons.calendarEnd
				trailingContent: Qaterial.TextFieldButtonContainer
				{
					Qaterial.TextFieldAlertIcon {  }
					Qaterial.TextFieldClearButton { }
				}

				onEditingFinished: recalcDuration()
			}

			QFormTextField {
				id: _name
				field: "name"
				title: qsTr("Munkakör")
				width: parent.width
				helperText: qsTr("A munkakör megnevezése")
				leadingIconSource: Qaterial.Icons.robotIndustrial
				trailingContent: Qaterial.TextFieldButtonContainer
				{
					Qaterial.TextFieldClearButton { }
				}
			}

			QFormSection {
				icon.source: Qaterial.Icons.accountTie
				text: qsTr("Munkáltató vagy megbízó")
			}

			QFormTextArea {
				id: _master
				field: "master"
				width: parent.width
				placeholderText: qsTr("A munkáltató vagy megbízó neve, címe, adószáma")
			}

			QFormSection {
				icon.source: Qaterial.Icons.fileDocumentEdit
				text: qsTr("Foglalkoztatási jogviszony")
			}

			QFormComboBox {
				id: _type
				field: "type"
				text: qsTr("Típusa:")
				model: App.jobTypeList
				combo.implicitWidth: Qaterial.Style.pixelSizeRatio*400
				combo.editable: false
				currentIndex: -1
			}

			QFormSpinBox {
				id: _hour
				field: "hour"
				text: qsTr("Heti munkaidő (óra):")
				from: 0
				to: 40
				spin.editable: true
			}

			QFormSpinBox {
				id: _value
				field: "value"
				text: qsTr("Heti óraszám (megbízás esetén):")
				from: 0
				to: 40
				spin.editable: true
			}

			QButton
			{
				anchors.horizontalCenter: parent.horizontalCenter
				action: _actionSave
			}


			QButton
			{
				anchors.horizontalCenter: parent.horizontalCenter
				action: _actionDelete
				bgColor: Qaterial.Colors.red500
				textColor: Qaterial.Colors.white
				visible: editData
			}
		}

	}


	Action {
		id: _actionSave
		text: qsTr("Kész")
		icon.source: Qaterial.Icons.checkBold
		enabled: _start.acceptableInput && _end.acceptableInput && _form.modified
		onTriggered:
		{
			let d = _form.getItems([_name, _master, _type, _hour, _value])

			let d1 = new Date(_start.text)
			d.start = d1.toLocaleDateString(Qt.locale(), "yyyy-MM-dd")

			if (_end.text !== "") {
				let d2 = new Date(_end.text)

				if (d2 < d1) {
					App.messageWarning(qsTr("A jogviszony vége a jogviszony kezdete előtt van!"), qsTr("Hibás dátum"))
					return
				}

				d.end = d2.toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
			} else {
				d.end = null
			}

			if (editData) {
				if (App.database.jobEdit(editData.id, d)) {
					App.snack(qsTr("Sikeres módosítás"))
					_form.modified = false
					App.stackPop()
				} else {
					App.messageError(qsTr("Sikertelen módosítás"))
				}
			} else {
				if (App.database.jobAdd(d)) {
					App.snack(qsTr("Új munkakör létrehozva"))
					_form.modified = false
					App.stackPop()
				} else {
					App.messageError(qsTr("Nem sikerült rögzíteni"))
				}
			}
		}
	}


	Action {
		id: _actionDelete
		text: qsTr("Törlés")
		icon.source: Qaterial.Icons.delete_
		enabled: editData
		onTriggered:
		{
			Qaterial.DialogManager.showDialog(
						{
							onAccepted: function()
							{
								if (App.database.jobDelete(editData.id)) {
									App.snack(qsTr("Sikeres törlés"))
									_form.modified = false
									App.stackPop()
								} else {
									App.messageError(qsTr("Törlés sikertelen"))
								}
							},
							text: qsTr("Biztosan törlöd a munkakört?"),
							title: editData.name,
							iconSource: Qaterial.Icons.delete_,
							iconColor: Qaterial.Colors.red500,
							textColor: Qaterial.Colors.red500,
							iconFill: false,
							iconSize: Qaterial.Style.roundIcon.size,
							standardButtons: DialogButtonBox.No | DialogButtonBox.Yes
						})
		}
	}


	function recalcDuration() {
		if (_start.text === "") {
			root.subtitle = ""
			return
		}

		let d1 = new Date(_start.text)
		let d2 = _end.text !== "" ? new Date(_end.text) : new Date()

		let y = App.yearsBetween(d1, d2)
		let d = App.daysBetween(d1, d2)

		root.subtitle = (y === 0 ? qsTr("%1 nap").arg(d) : qsTr("%1 év %2 nap").arg(y).arg(d))

	}

	Component.onCompleted: {
		if (editData) {

			let x = editData.start.toLocaleDateString(Qt.locale(), "yyyy-MM-dd")

			_start.fieldData = editData.start.toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
			if (editData.end) {
				_end.text = editData.end.toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
			}

			_form.setItems([_name, _master, _type, _hour, _value], editData)
		}

		recalcDuration()
	}

	StackView.onActivated: _start.forceActiveFocus()

}
