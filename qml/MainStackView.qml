import QtQuick
import QtQuick.Controls
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

Qaterial.StackView
{
	id: mainStackView

	signal stackViewLoaded()

	initialItem: Qaterial.LabelBody2 {
		//anchors.centerIn: parent
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		text: qsTr("Betöltés...")
		Component.onCompleted: stackViewLoaded()
	}

	Transition {
		id: transitionEnter

		PropertyAnimation {
			property: "opacity"
			from: 0.0
			to: 1.0
		}
	}

	Transition {
		id: transitionExit

		PropertyAnimation {
			property: "opacity"
			from: 1.0
			to: 0.0
		}
	}

	pushEnter: transitionEnter
	pushExit: transitionExit
	popEnter: transitionEnter
	popExit: transitionExit


	function createPage(_qml : string, _prop : jsobject) : Item {
		var cmp = Qt.createComponent(_qml, Component.PreferSynchronous)

		if (!cmp || cmp.status !== Component.Ready) {
			console.warn("Can't create component:", cmp.errorString())
			return null
		}

		return mainStackView.push(cmp, _prop)
	}

	function popPage(_index : int) {
		if (_index < 0 || _index >= depth) {
			console.warn(qsTr("Invalid index"), index)
			return
		}

		if (currentItem.onPageClose) {
			console.debug(qsTr("Lap bezárási funkció meghívása:"), currentItem)
			currentItem.onPageClose()
		}

		pop(get(_index))
	}



	function callStackPop() : bool {
		if (currentItem.StackView.status !== StackView.Active) {
			console.debug(qsTr("StackView.status != Active"), currentItem.StackView.status)
			return false
		}

		if (currentItem.stackPopFunction) {
			console.debug(qsTr("Lap pop funkció meghívása:"), currentItem)
			return currentItem.stackPopFunction()
		}

		return true
	}

}
