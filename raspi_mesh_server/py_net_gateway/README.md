# MQTT client
### https://pypi.python.org/pypi/paho-mqtt/1.1
```
sudo pip install paho-mqtt
```

# Milight client
### https://github.com/McSwindler/python-milight
```
sudo pip install milight
```

# Wemo Switch insight
### https://github.com/iancmcc/ouimeaux
the documentation is referring to another fork, so I rather installed from the original repo
```
sudo pip install git+https://github.com/iancmcc/ouimeaux.git
```

### explain result
switch insight functions:
```
insight.today_kwh
insight.current_power
insight.today_on_time
insight.on_for
insight.today_standby_time
```

switch.explain() run on a wemo insight switch returns the following functions:
```
smartsetup
----------
  GetRegistrationStatus(RegistrationStatus, StatusCode)
  GetRegistrationData(SmartDeviceData, RegistrationData)
  PairAndRegister(RegistrationData, PairingStatus, PairingData)
()
manufacture
-----------
  GetManufactureData(ManufactureData)
()
rules
-----
  SetRulesDBVersion(RulesDBVersion)
  GetRulesDBPath(RulesDBPath)
  GetRulesDBVersion(RulesDBVersion)
  SimulatedRuleData(DeviceList, DeviceCount)
  FetchRules(ruleDbPath, ruleDbVersion, errorInfo)
  UpdateWeeklyCalendar(Wed, Sun, Fri, Thurs, Tues, Mon, Sat)
  SetRuleID(RuleFreq, RuleMsg, RuleID)
  DeleteRuleID(RuleID)
  EditWeeklycalendar(action)
  StoreRules(processDb, ruleDbBody, errorInfo, ruleDbVersion)
()
remoteaccess
------------
  RemoteAccess(DeviceName, smartprivateKey, MacAddr, dst, ArpMac, smartUniqueId, numSmartDev, DeviceId, pluginprivateKey, HomeId)
()
insight
-------
  GetTodaySBYTime(TodaySBYTime)
  ResetPowerThreshold(PowerThreshold)
  SetPowerThreshold(PowerThreshold)
  GetPowerThreshold(PowerThreshold)
  ScheduleDataExport(DataExportType, EmailAddress)
  GetTodayKWH(TodayKWH)
  GetDataExportInfo(EmailAddress, DataExportType, LastDataExportTS)
  GetInsightParams(InsightParams)
  GetTodayONTime(TodayONTime)
  GetONFor(ONFor)
  GetInsightInfo(InsightInfo)
  SetAutoPowerThreshold(PowerThreshold)
  GetInSBYSince(InSBYSince)
  GetPower(InstantPower)
()
deviceinfo
----------
  GetInformation(Information, UTC, dst, DstSupported, TimeZone, TimeSync)
  UpdateBridgeList(BridgeList)
  OpenInstaAP()
  CloseInstaAP()
  GetDeviceInformation(UTC, DeviceInformation, dst, DstSupported, TimeZone, TimeSync, CountdownTime)
  GetConfigureState(ConfigureState)
  InstaConnectHomeNetwork(encrypt, ssid, auth, brlist, password, channel)
  GetRouterInformation(encrypt, ssid, auth, mac, password, channel)
  InstaRemoteAccess(DeviceName, smartprivateKey, MacAddr, dst, smartUniqueId, numSmartDev, DeviceId, pluginprivateKey, HomeId)
()
metainfo
--------
  GetMetaInfo(MetaInfo)
  GetExtMetaInfo(ExtMetaInfo)
()
WiFiSetup
---------
  GetNetworkStatus(NetworkStatus)
  CloseSetup()
  ConnectHomeNetwork(password, encrypt, ssid, auth, channel)
  GetApList(ApList)
  GetNetworkList(NetworkList)
  StopPair()
()
firmwareupdate
--------------
  UpdateFirmware(WithUnsignedImage, URL, ReleaseDate, Signature, NewFirmwareVersion, DownloadStartTime)
  GetFirmwareVersion(FirmwareVersion)
()
timesync
--------
  GetTime()
  TimeSync(UTC, dst, DstSupported, TimeZone)
()
basicevent
----------
  GetCrockpotState(mode, time)
  SetSmartDevInfo(SmartDevURL)
  SetServerEnvironment(ServerEnvironmentType, TurnServerEnvironment, ServerEnvironment)
  GetDeviceId()
  ControlCloudUpload(EnableUpload)
  GetRuleOverrideStatus(RuleOverrideStatus)
  NotifyManualToggle()
  GetIconURL(URL)
  SetBinaryState(BinaryState, CountdownEndTime, EndAction, deviceCurrentTime, Duration, UDN)
  SetLogLevelOption(Option, Level)
  GetHomeInfo()
  GetMacAddr()
  GetLogFileURL(LOGURL)
  ReSetup(Reset)
  GetDeviceIcon(DeviceIcon)
  ShareHWInfo(RestoreState, PluginKey, Mac, HomeId, Serial, Udn)
  UpdateInsightHomeSettings(EnergyPerUnitCostVersion, Currency, EnergyPerUnitCost)
  SetCrockpotState(mode, time)
  GetSmartDevInfo()
  GetBinaryState(BinaryState)
  GetSimulatedRuleData()
  ChangeDeviceIcon(PictureSize, PictureColorDeep, PictureHeight)
  SetDeviceId(SetDeviceId)
  GetHomeId()
  GetServerEnvironment(ServerEnvironmentType, TurnServerEnvironment, ServerEnvironment)
  ChangeFriendlyName(FriendlyName)
  GetFriendlyName(FriendlyName)
  GetInsightHomeSettings(HomeSettingsVersion, EnergyPerUnitCost, Currency)
  GetWatchdogFile(WDFile)
  SetInsightHomeSettings(Currency, EnergyPerUnitCost)
  GetSignalStrength(SignalStrength)
  SetMultiState(state)
  SetHomeId(HomeId)
  GetIconVersion(IconVersion)
  GetPluginUDN()
  GetSerialNo()
()
```
