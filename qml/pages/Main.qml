/*
    Copyright (C) 2018 Michał Szczepaniak

    This file is part of Microtube.

    Microtube is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Microtube is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Microtube.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0
import com.verdanditeam.yt 1.0
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All
    backNavigation: false
    property bool initialized: false

    onStatusChanged: {
        if (!initialized) {
            swipeView.contentX = page.width;
            initialized = true;
        }
    }

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property bool autoPlay: true
        property bool audioOnlyMode: false
        property bool developerMode: false
        property double buffer: 1.0
        property string categoryId: "0"
        property string categoryName: "Film & Animation"
        property int maxDefinition: 1080
        property int currentRegionId: 0
        property string currentRegion: ""
    }


    YtCategories {
        id: categories
    }

    ChannelHelper {
        id: channelHelper
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("About.qml"))
                }
            }

            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Settings.qml"))
                }
            }

            MenuItem {
                text: qsTr("Subscriptions")
                onClicked: {
                    subscriptionsAggregator.updateSubscriptions()
                    pageStack.push(Qt.resolvedUrl("Subscriptions.qml"), {playlistModel: app.playlistModel})
                }
            }

            MenuItem {
                text: qsTr("Filters")
                enabled: typeof playlistModel.searchParams !== "undefined"
                visible: false
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Filters.qml"), {playlistModel: app.playlistModel})
                }
            }
        }

        PageHeader {
            id: header
            title: qsTr("Search")
        }

        SearchField {
            id: searchField
            width: parent.width
            anchors.top: header.bottom
            placeholderText: qsTr("Search")
            Keys.onReturnPressed: {
                if(searchField.text.length != 0) {
                    if(searchField.text == "21379111488") settings.developerMode = !settings.developerMode
                    playlistModel.search(searchField.text)
                }
            }
            Component.onCompleted: {
                if (Qt.application.arguments.length === 2) {
                    playlistModel.search(Qt.application.arguments[1])
                    searchField.text = Qt.application.arguments[1]
                }
            }
        }


        SilicaListView {
            id: swipeView

            clip: true
            orientation: ListView.Horizontal
            layoutDirection: ListView.LeftToRight

            anchors.top: searchField.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            interactive: true
            contentHeight: height
            contentWidth: width*2
            snapMode: ListView.SnapOneItem

            Behavior on contentX { PropertyAnimation {} }

            model: ListModel {
                id: listModel

                ListElement {}
                ListElement {}
            }

            delegate: Item {
                width: swipeView.width
                height: swipeView.height

                SilicaFastListView {
                    id: browseList
                    width: swipeView.width
                    height: swipeView.height
                    clip: true
                    model: categories
                    visible: index === 0

                    delegate: CategoryElement {
                        onClicked: {
                            playlistModel.loadCategory(name, settings.currentRegion)
                            swipeView.contentX = -page.width
                        }
                    }
                }

                SilicaFastListView {
                    id: searchList
                    width: swipeView.width
                    height: swipeView.height
                    clip: true
                    spacing: Theme.paddingMedium
                    visible: index === 1
                    model: playlistModel

                    Label {
                        anchors.centerIn: parent
                        width: parent.width/2
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        horizontalAlignment: Text.AlignHCenter
                        text: qsTr("Select category by swiping to the left or search for videos")
                        visible: !searchList.count
                    }

                    delegate: VideoElement {
                        onClicked: {
                            if (elementType === YtPlaylist.ChannelType) {
                                pageStack.push(Qt.resolvedUrl("Channel.qml"), {channelId: id })
                            } else {
                                if (pageStack.nextPage(page))
                                    pageStack.popAttached(page, PageStackAction.Immediate)
                                pageStack.pushAttached(Qt.resolvedUrl("VideoPlayer.qml"), {videoIdToPlay: id})
                                pageStack.navigateForward()
                            }
                        }
                    }
                }
            }
        }
    }
}
