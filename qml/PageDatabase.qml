import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial
import "JScript.js" as JS

QPage {
	id: control

	closeQuestion: App.database && App.database.modified ? qsTr("Biztosan bezárod?") : ""

	title: App.database ? App.database.title : "???"

	appBar.rightComponent: Qaterial.AppBarButton
	{
		icon.source: Qaterial.Icons.dotsVertical
		onClicked: menu.open()

		QMenu {
			id: menu

			QMenuItem { action: actionRename }
			QMenuItem { action: actionImport }
			Qaterial.MenuSeparator {}
			QMenuItem { action: actionSave }
			QMenuItem { action: _actionPrint }
			QMenuItem { action: actionClose }
		}
	}


	Action {
		id: actionRename
		text: qsTr("Név megadása")
		icon.source: Qaterial.Icons.account
		enabled: App.database
		onTriggered: {
			Qaterial.DialogManager.showTextFieldDialog({
														   textTitle: qsTr("Munkavállaló neve"),
														   title: qsTr("Név megadása"),
														   text: App.database.title,
														   standardButtons: DialogButtonBox.Cancel | DialogButtonBox.Ok,
														   onAccepted: function(_text, _noerror) {
															   App.database.title = _text
														   }
													   })
		}
	}


	Action {
		id: actionSave
		text: qsTr("Mentés")
		icon.source: Qaterial.Icons.contentSave
		enabled: App.database
		shortcut: "Ctrl+S"
		onTriggered: {
			App.dbSave()
		}
	}


	Action {
		id: actionClose
		text: qsTr("Bezárás")
		icon.source: Qaterial.Icons.close
		onTriggered: App.stackPop()
	}

	Action {
		id: _actionPrint
		text: qsTr("PDF")
		icon.source: Qaterial.Icons.filePdf
		enabled: App.database
		shortcut: "Ctrl+P"
		onTriggered: {
			App.dbPrint()
		}
	}

	Action {
		id: actionImport
		text: qsTr("Importálás")
		icon.source: Qaterial.Icons.import_
		onTriggered: App.stackPushPage("PageImport.qml")
	}



	QScrollable {
		id: _scrollable

		anchors.fill: parent
		horizontalPadding: 0
		topPadding: 0
		bottomPadding: 0
		spacing: 5

		refreshEnabled: App.database
		onRefreshRequest: App.database.sync()

		Qaterial.IconLabel {
			font: Qaterial.Style.textTheme.headline6
			color: Qaterial.Style.iconColor()
			icon.source: Qaterial.Icons.briefcaseVariant
			anchors.left: _view.left
			width: _view.width
			horizontalAlignment: Qt.AlignLeft
			text: App.database ? qsTr("Jelenlegi jogviszony (piarista): %1 év %2 nap\n").arg(App.database.calculation.jobYears).arg(App.database.calculation.jobDays) : ""
		}

		Qaterial.IconLabel {
			font: Qaterial.Style.textTheme.headline6
			color: Qaterial.Style.iconColor()
			icon.source: Qaterial.Icons.hammerWrench
			anchors.left: _view.left
			width: _view.width
			horizontalAlignment: Qt.AlignLeft
			text: App.database ? qsTr("Szakmai gyakorlat: %1 év %2 nap\n").arg(App.database.calculation.practiceYears).arg(App.database.calculation.practiceDays) : ""
		}

		Qaterial.IconLabel {
			font: Qaterial.Style.textTheme.headline6
			color: Qaterial.Style.iconColor()
			icon.source: Qaterial.Icons.medal
			anchors.left: _view.left
			width: _view.width
			horizontalAlignment: Qt.AlignLeft
			text: App.database ? qsTr("Jubileumi jutalom: %1 év %2 nap").arg(App.database.calculation.prestigeYears).arg(App.database.calculation.prestigeDays) : ""
		}

		Row {
			anchors.left: _view.left

			spacing: 10

			Qaterial.LabelCaption {
				text: qsTr("Számított időtartamok csökkentése ide:")
				anchors.verticalCenter: parent.verticalCenter
			}

			Qaterial.ComboBox {
				id: _combo1
				anchors.verticalCenter: parent.verticalCenter
				font: Qaterial.Style.textTheme.body2

				width: 200

				model: [
					{ value: -1, text: qsTr("Mai dátum") },
					{ value: 20240101, text: new Date(2024, 0, 1).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.") }
				]

				textRole: "text"
				valueRole: "value"
				onActivated: {
					App.database.prestigeCalculationTime = currentValue
					App.database.sync()
				}

				currentIndex:  {
					if (!App.database)
						return -1

					for (let n=0; n<model.length; ++n) {
						if (model[n].value === App.database.prestigeCalculationTime)
							return n
					}

					return -1
				}
			}
		}

		Qaterial.IconLabel {
			font: Qaterial.Style.textTheme.headline6
			color: Qaterial.Style.accentColor
			icon.source: Qaterial.Icons.calendarStar
			anchors.left: _view.left
			width: _view.width
			horizontalAlignment: Qt.AlignLeft
			text: App.database ? qsTr("Következő jubileumi jutalom időpontja: %1 (%2 év)")
								 .arg(App.database.calculation.nextPrestigeYears > 0 ?
										  App.database.calculation.nextPrestige.toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")
										: "-")
								 .arg(App.database.calculation.nextPrestigeYears > 0 ? App.database.calculation.nextPrestigeYears : "-")
							   : ""
		}

		Qaterial.HorizontalLineSeparator {
			anchors.horizontalCenter: parent.horizontalCenter
			width: _view.width
		}

		ListView {
			id: _view

			height: contentHeight

			width: Math.min(parent.width, Qaterial.Style.maxContainerSize)
			anchors.horizontalCenter: parent.horizontalCenter

			boundsBehavior: Flickable.StopAtBounds

			model: App.database ? App.database.model : null

			delegate: Qaterial.LoaderItemDelegate {
				width: ListView.view.width

				leftSourceComponent: Qaterial.Icon {
					anchors.verticalCenter: parent.verticalCenter
					icon: model.overlap ? Qaterial.Icons.alert : Qaterial.Icons.check
					color: model.overlap ? Qaterial.Style.accentColor : Qaterial.Colors.green400
				}


				text: model.name
				secondaryText: new Date(model.start).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")
							   + (model.end ? (" - " + new Date(model.end).toLocaleDateString(Qt.locale(), "yyyy. MMMM d.")) : "")
							   + qsTr(" (%1 év %2 nap)").arg(model.durationYears).arg(model.durationDays)



				rightSourceComponent: Row {
					spacing: 5

					anchors.verticalCenter: parent.verticalCenter

					Row {
						spacing: 10
						anchors.verticalCenter: parent.verticalCenter

						CalculationLabel {
							anchors.verticalCenter: parent.verticalCenter
							mode: model.jobMode
							years: model.jobYears
							days: model.jobDays
							icon.source: Qaterial.Icons.briefcaseVariant
						}

						CalculationLabel {
							anchors.verticalCenter: parent.verticalCenter
							mode: model.practiceMode
							years: model.practiceYears
							days: model.practiceDays
							icon.source: Qaterial.Icons.hammerWrench
						}

						CalculationLabel {
							anchors.verticalCenter: parent.verticalCenter
							mode: model.prestigeMode
							years: model.prestigeYears
							days: model.prestigeDays
							icon.source: Qaterial.Icons.medal
						}
					}

					Qaterial.RoundButton {
						anchors.verticalCenter: parent.verticalCenter
						icon.source: Qaterial.Icons.pencil
						foregroundColor: Qaterial.Style.iconColor()
						onClicked: {
							App.stackPushPage("PageJobEdit.qml", {
												  editData: model
											  })
						}
					}
				}


				onClicked: {
					App.stackPushPage("PageCalculationEdit.qml", {
										  editData: model
									  })
				}
			}
		}
	}

	QFabButton {
		visible: App.database
		action: _actionJobAdd
	}


	Action {
		id: _actionJobAdd
		text: qsTr("Új")
		icon.source: Qaterial.Icons.plus
		onTriggered: {
			App.stackPushPage("PageJobEdit.qml")
		}
	}

	Component.onDestruction: App.dbClose()
}
