import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial
import TimeCalculator
import Qt5Compat.GraphicalEffects
import "JScript.js" as JS

Qaterial.ApplicationWindow
{
	id: mainWindow
	width: 640
	height: 480

	visible: true

	minimumWidth: 320
	minimumHeight: 240

	color: "black"

	property bool _completed: false
	readonly property bool _allLoaded: mainStackView._loaded && _completed


	MainStackView {
		id: mainStackView
		anchors.fill: parent

		property bool _loaded: false

		onStackViewLoaded: _loaded = true
	}


	Component.onCompleted:
	{
		JS.intializeStyle()

		Qaterial.Style.dialog.implicitWidth = Qt.binding(function() {
			return Math.min(mainWindow.width*.9, 400 * Qaterial.Style.pixelSizeRatio)
		})

		App.mainStack = mainStackView
		App.mainWindow = mainWindow

		_completed = true
	}


	on_AllLoadedChanged: if (_allLoaded) App.onApplicationStarted()


	Shortcut
	{
		sequences: ["Esc", "Back"]
		onActivated:
		{
			App.stackPop()
		}
	}

/*
	property Action fontPlus: Action {
		shortcut: "Ctrl++"
		text: qsTr("Növelés")
		icon.source: Qaterial.Icons.magnifyPlus
		onTriggered: setPixelSize(Qaterial.Style.userPixelSize+1)
	}

	property Action fontMinus: Action {
		shortcut: "Ctrl+-"
		text: qsTr("Csökkentés")
		icon.source: Qaterial.Icons.magnifyMinus
		onTriggered: setPixelSize(Qaterial.Style.userPixelSize-1)
	}

	property Action fontNormal: Action {
		shortcut: "Ctrl+0"
		text: qsTr("Visszaállítás")
		icon.source: Qaterial.Icons.magnifyRemoveOutline
		onTriggered: setPixelSize(Qaterial.Style.defaultPixelSize)
	}
*/

	property Action actionFullScreen: Action {
		shortcut: "Ctrl+F11"
		text: qsTr("Teljes képernyő")
		icon.source: App.fullScreenHelper ? Qaterial.Icons.fullscreenExit : Qaterial.Icons.fullscreen
		checked: App.fullScreenHelper
		checkable: true

		onTriggered: App.fullScreenHelper = checked
	}



	onWidthChanged: {
		if (width >= 992 * Qaterial.Style.pixelSizeRatio)
			Qaterial.Style.maxContainerSize = 970 * Qaterial.Style.pixelSizeRatio
		else if (width >= 768 * Qaterial.Style.pixelSizeRatio)
			Qaterial.Style.maxContainerSize = 750 * Qaterial.Style.pixelSizeRatio
		else
			Qaterial.Style.maxContainerSize = width

	}

	onClosing: close => {
		if (App.closeWindow()) {
			if (mainStackView.currentItem.onPageClose) {
				console.info(qsTr("Lap bezárási funkció meghívása:"), mainStackView.currentItem)
				mainStackView.currentItem.onPageClose()
			}
			close.accepted = true
			Qt.quit()
		} else {
			close.accepted = false
		}
	}



	function messageDialog(_text : string, _title : string, _type : string) {
		var _icon = Qaterial.Icons.informationOutline
		var _color = Qaterial.Style.primaryTextColor()

		if (_type === "warning") {
			_icon = Qaterial.Icons.alert
			_color = Qaterial.Colors.orange500
		} else if (_type === "error") {
			_icon = Qaterial.Icons.alertOctagon
			_color = Qaterial.Colors.red600
		}

		Qaterial.DialogManager.showDialog(
					{
						text: _text,
						title: _title,
						iconSource: _icon,
						iconColor: _color,
						textColor: _color,
						iconFill: false,
						iconSize: Qaterial.Style.roundIcon.size,
						standardButtons: DialogButtonBox.Ok
					})
	}


	function closeQuestion(_text : string, _pop : bool, _index: int) {
		Qaterial.DialogManager.showDialog(
					{
						onAccepted: function()
						{
							if (_pop)
								App.stackPop(_index, true)
							else
								App.closeWindow(true)
						},
						text: _text,
						title: _pop ? qsTr("Bezárás") : qsTr("Kilépés"),
						iconSource: Qaterial.Icons.closeCircle,
						iconColor: Qaterial.Colors.orange500,
						textColor: Qaterial.Colors.orange500,
						iconFill: false,
						iconSize: Qaterial.Style.roundIcon.size,
						standardButtons: DialogButtonBox.No | DialogButtonBox.Yes
					})
	}


	function snack(_text : string) {
		Qaterial.SnackbarManager.show(
					{
						text: _text,
					}

					)
	}


	function setPixelSize(newSize) {
		if (newSize >= Qaterial.Style.defaultPixelSize/2.5 && newSize <= Qaterial.Style.defaultPixelSize * 3.0) {
			Qaterial.Style.userPixelSize = newSize
			App.Utils.settingsSet("window/fontSize", newSize)
		}

	}

}

