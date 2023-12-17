import QtQuick
import QtQuick.Controls
import Qaterial as Qaterial
import "./QaterialHelper" as Qaterial

Qaterial.IconLabel {
	id: root

	property int mode: -1
	property int years: 0
	property int days: 0

	font: Qaterial.Style.textTheme.caption
	icon.source: Qaterial.Icons.exitToApp

	color: {
		switch (mode) {
		case 0:
			return Qaterial.Colors.red400
		case 1:
			return Qaterial.Colors.green400
		case 2:
			return Qaterial.Colors.amber400
		default:
			return Qaterial.Style.secondaryTextColor()
		}
	}

	text: mode == 2 ? qsTr("%1 év %2 nap").arg(years).arg(days) : ""

}
