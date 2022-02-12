kcoreaddons_add_plugin(tmsuplugin SOURCES tmsuplugin.cpp INSTALL_NAMESPACE "kf5/kfileitemaction")
kcoreaddons_desktop_to_json(tmsuplugin tmsuplugin.desktop)

target_sources(tmsuplugin PRIVATE
    tmsuplugin.cpp
)

kconfig_add_kcfg_files(tmsuplugin
    tmsupluginsettings.kcfgc
)

target_link_libraries(tmsuplugin
    KF5::XmlGui
    KF5::KIOWidgets
)

install(FILES tmsupluginsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
