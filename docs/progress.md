# Decompilation progress

The primary milestone is a playable rebuild. Instruction accuracy remains
visible, but does not take precedence over startup and gameplay dependencies.
Reviewed semantically identical near matches (initially at least 95% raw) are
implemented functions, counted separately from exact matches.

Latest combined Python/SP5/Wine and CMake/NMake verification: 2026-10-19.
The match guard verified **349 exact functions and sixteen reviewed near
matches**. Another 812 reconstructions remain `STUB`; both builders agree on
all 1,177 measured raw scores and annotation statuses. Data comparison verifies
218 annotated globals with zero issues. Six genuine folded aliases also have
complete-byte and exact linker-map verification. Unlinked inventory entries
may appear in the CLI denominator without another measured row.

This is the reconstructed subset, not whole-game coverage. The automatic
Ghidra inventory has **4,459 discovered function starts**, including **22 thunks**
and linked runtime/library code. It is incomplete and is not a verified count
of game-authored functions; see [inventory notes](research.md#automatic-function-inventory).
Identified libraries are excluded from these matches with `--nolib`.
The executable still uses the temporary bootstrap and has not launched a menu.
See [frontend integration](work-controller-frontend.md), the
[runtime roadmap](runtime-roadmap.md), and [library identification](library-identification.md).

| Original VA | Function | Instruction accuracy | Status |
| --- | --- | ---: | --- |
| `0x00403380` | `LoadDefaultPlayerCar` | 63.83% | Work in progress |
| `0x00403910` | `IsApplicationDevelopmentMode` | 100.00% | Verified exact |
| `0x00403930` | `OnApplicationDiagnostic` | 100.00% | Verified exact |
| `0x004047a0` | `GameObjectBaseZeroOneArgument` | 100.00% | Work in progress |
| `0x00406f90` | `InitializeGameLocalizationPath` | 22.22% | Work in progress |
| `0x004071a0` | `GameControllerScore::Initialize` | 100.00% | Verified exact |
| `0x004071f0` | `GameControllerPairGroups::Initialize` | 22.64% | Work in progress |
| `0x00407f30` | `BodypartManagerGetObjectSize` | 100.00% | Verified exact |
| `0x00409190` | `GameControllerItem::Initialize` | 100.00% | Verified exact |
| `0x0040a000` | `InitializeControllerBindingProfile` | 39.72% | Work in progress |
| `0x0040a230` | `ApplyControllerFonts` | 100.00% | Verified exact |
| `0x0040a290` | `StartControllerMusic` | 57.14% | Work in progress |
| `0x0040a3d0` | `InitializeLocalizedPlayerNames` | 42.42% | Work in progress |
| `0x0040a540` | `AccessGameLanguagePreference` | 53.12% | Work in progress |
| `0x0040a600` | `GetRegistryMediaPath` | 55.93% | Work in progress |
| `0x0040a6d0` | `HasCdLanguageExport` | 65.31% | Work in progress |
| `0x0040a760` | `ConfigureGameScreenshotDirectory` | 94.87% | Work in progress |
| `0x0040a7d0` | `BuildUserDataPath` | 100.00% | Verified exact |
| `0x0040a970` | `InitializeGameControllerDefaults` | 51.96% | Work in progress |
| `0x0040abc0` | `ResetGameControllerSession` | 95.83% | Reviewed near match |
| `0x0040ac30` | `InitializeGameControllerPlayers` | 29.20% | Work in progress |
| `0x0040ad30` | `GameControllerSettings::SetPlayerName` | 86.49% | Work in progress |
| `0x0040aef0` | `GameControllerName::Initialize` | 100.00% | Verified exact |
| `0x0040af10` | `GameControllerName::SetNarrowText` | 100.00% | Verified exact |
| `0x0040af30` | `GameControllerName::SetWideText` | 69.23% | Work in progress |
| `0x0040b000` | `GameControllerSelection::Initialize` | 9.09% | Work in progress |
| `0x0040b0a0` | `GameControllerSelection::UnlockWorld` | 50.00% | Work in progress |
| `0x0040b0d0` | `GameControllerSelection::UnlockTrack` | 94.74% | Work in progress |
| `0x0040b0f0` | `GameControllerSelection::IsTrackUnlocked` | 96.00% | Reviewed near match |
| `0x0040b160` | `GameControllerSelection::UnlockReward` | 24.00% | Work in progress |
| `0x0040b1b0` | `GameControllerSelection::HasReward` | 96.00% | Reviewed near match |
| `0x0040b2c0` | `GameControllerSelection::UnlockDefaults` | 87.93% | Work in progress |
| `0x0040b340` | `InitializeGameControllerWord` | 100.00% | Verified exact |
| `0x0040ec90` | `Application::GetDisplaySettings` | 100.00% | Verified exact |
| `0x00410380` | `GetScreenPrinterSize` | 100.00% | Verified exact |
| `0x00417d10` | `MenuPanel::MenuPanel` | 100.00% | Verified exact |
| `0x00417d40` | `MenuPanel::Initialize` | 25.00% | Work in progress |
| `0x00417e30` | `MenuPanel::Release` | 100.00% | Verified exact |
| `0x00417e40` | `MenuPanel::Activate` | 27.51% | Work in progress |
| `0x00417fd0` | `MenuPanel::AllocateSprites` | 42.23% | Work in progress |
| `0x004181c0` | `MenuPanel::ReleaseSprites` | 100.00% | Verified exact |
| `0x00418230` | `MenuPanel::BeginClosing` | 100.00% | Verified exact |
| `0x00418260` | `MenuPanel::Deactivate` | 100.00% | Verified exact |
| `0x00418280` | `MenuPanel::Update` | 52.59% | Work in progress |
| `0x004187f0` | `MenuPanel::UpdateClipping` | 36.36% | Work in progress |
| `0x004189a0` | `MenuPanel::ResetGeometry` | 48.65% | Work in progress |
| `0x00418a10` | `MenuPanel::SetTransitionVelocity` | 28.40% | Work in progress |
| `0x00418ba0` | `MenuPanel::UpdateSpriteGeometry` | 15.33% | Work in progress |
| `0x00418fd0` | `MenuPanel::QuerySelection` | 88.89% | Work in progress |
| `0x00419090` | `MenuPanel::QueryTargetGeometry` | 23.18% | Work in progress |
| `0x00419410` | `MenuPanelSet::Initialize` | 49.38% | Work in progress |
| `0x00419470` | `MenuPanelSet::Release` | 89.66% | Work in progress |
| `0x004194c0` | `MenuPanelSet::Begin` | 77.78% | Work in progress |
| `0x004194f0` | `MenuPanelSet::End` | 80.00% | Work in progress |
| `0x00419680` | `MenuPanelSet::SetActive` | 83.64% | Work in progress |
| `0x0041c310` | `MenuManager::MenuManager` | 82.22% | Work in progress |
| `0x0041c3f0` | `MenuManagerAllocationSizeBridge` | 100.00% | Work in progress |
| `0x0041c400` | `MenuManagerDestroyBridge` | 100.00% | Work in progress |
| `0x0041c420` | `MenuManager::~MenuManager` | 50.00% | Work in progress |
| `0x0041c430` | `MenuManager::Initialize` | 22.58% | Work in progress |
| `0x0041c4d0` | `MenuManager::Remove` | 100.00% | Work in progress |
| `0x0041c6d0` | `MenuManager::ApplyFonts` | 71.70% | Work in progress |
| `0x00423090` | `MenuManager::GetPanelRectangle` | 100.00% | Verified exact |
| `0x004230d0` | `MenuManager::GetSecondaryPanelRectangle` | 100.00% | Verified exact |
| `0x004236a0` | `MenuManager::SetPanelClipRectangle` | 77.42% | Work in progress |
| `0x00423710` | `MenuManager::SetSecondaryPanelClipRectangle` | 93.51% | Work in progress |
| `0x00423d00` | `MenuTextPair::SetClipRectangle` | 100.00% | Verified exact |
| `0x00423d30` | `MenuTextPair::GetRectangle` | 76.06% | Work in progress |
| `0x00427910` | `ApplySavedAudioLevels` | 40.00% | Work in progress |
| `0x00427970` | `ApplySavedAudioQuality` | 76.92% | Work in progress |
| `0x0042ce60` | `BitmapFontSet::BitmapFontSet` | 100.00% | Verified exact |
| `0x0042ce80` | `BitmapFontSet::~BitmapFontSet` | 0.00% | Work in progress |
| `0x0042cef0` | `BitmapFontSet::Initialize` | 23.08% | Work in progress |
| `0x0042d020` | `BitmapFontSet::Release` | 18.39% | Work in progress |
| `0x004334c0` | `GlobalOverlayGlyphReplacementManagerView::Initialize` | 39.20% | Work in progress |
| `0x00433640` | `GlobalOverlayGlyphReplacementManagerView::Release` | 76.47% | Work in progress |
| `0x00433680` | `GlobalOverlayGlyphReplacementManagerView::LoadIconTextures` | 100.00% | Verified exact |
| `0x00433700` | `GlobalOverlayGlyphReplacementManagerView::ReleaseIconTextures` | 100.00% | Verified exact |
| `0x00433750` | `GlobalOverlayGlyphReplacementManagerView::SetPlayerName` | 100.00% | Verified exact |
| `0x004338c0` | `ConstructControllerSelectionState` | 100.00% | Verified exact |
| `0x00434150` | `ReadSignedClockSeconds` | 47.06% | Work in progress |
| `0x00434240` | `ConstructControllerEventRecords` | 18.18% | Work in progress |
| `0x00434bc0` | `LoadControllerHudTextures` | 77.53% | Work in progress |
| `0x00434d90` | `ControllerEventRecordTable::SetPixelTextureRegion` | 82.61% | Work in progress |
| `0x00434e00` | `ControllerEventRecordTable::SetFullTextureRegion` | 100.00% | Verified exact |
| `0x00434e30` | `ReleaseControllerHudTextures` | 100.00% | Verified exact |
| `0x00435c40` | `LoadControllerPauseMenuTextures` | 16.82% | Work in progress |
| `0x00436070` | `ReleaseControllerPauseMenuTextures` | 100.00% | Verified exact |
| `0x004360d0` | `LoadControllerBuilderTextures` | 13.85% | Work in progress |
| `0x00436af0` | `ReleaseControllerBuilderTextures` | 15.14% | Work in progress |
| `0x0043dcd0` | `GetConfiguredAudioServiceMode` | 85.71% | Work in progress |
| `0x0044b3b0` | `DeleteConstraint` | 78.26% | Work in progress |
| `0x0044cd00` | `BodypartManagerInitialize` | 8.00% | Work in progress |
| `0x004534e0` | `GameObjectBaseDestroy` | 100.00% | Work in progress |
| `0x0045dcb0` | `FUN_0045dcb0` | 100.00% | Verified exact |
| `0x0045dda0` | `IsTokenCharacter` | 100.00% | Verified exact |
| `0x0045de00` | `IsHorizontalSpace` | 100.00% | Verified exact |
| `0x0045de20` | `FUN_0045de20` | 57.94% | Work in progress |
| `0x0045df40` | `FUN_0045df40` | 100.00% | Verified exact |
| `0x0045dfd0` | `FUN_0045dfd0` | 100.00% | Verified exact |
| `0x0045f770` | `ScreenPrinter::ScreenPrinter` | 61.54% | Work in progress |
| `0x0045f7a0` | `ScreenPrinter::Initialize` | 34.00% | Work in progress |
| `0x0045f870` | `ScreenPrinter::Remove` | 100.00% | Verified exact |
| `0x0045f8a0` | `ScreenPrinter::Update` | 82.93% | Work in progress |
| `0x0045f8d0` | `ScreenPrinter::ApplyFont` | 87.18% | Work in progress |
| `0x0045f900` | `ScreenPrinter::SetText` | 43.48% | Work in progress |
| `0x0045f930` | `ScreenPrinter::ClearText` | 100.00% | Verified exact |
| `0x00466470` | `InitializeCharacterPartPresets` | 1.47% | Work in progress |
| `0x00467380` | `CharacterPartPreset::Set` | 100.00% | Verified exact |
| `0x004673e0` | `CharacterPartPreset::Clear` | 53.85% | Work in progress |
| `0x00479490` | `RendererDefaultRecord::RendererDefaultRecord` | 45.71% | Work in progress |
| `0x00493550` | `FilterApplicationWindowMessage` | 100.00% | Verified exact |
| `0x00494f40` | `ApplicationCharacterQueue::QueueCharacter` | 100.00% | Verified exact |
| `0x0049a610` | `Application::Application` | 90.91% | Work in progress |
| `0x0049a6a0` | `Application::GetUnknownFlag` | 100.00% | Verified exact |
| `0x0049a760` | `GetApplication` | 57.14% | Work in progress |
| `0x0049a7a0` | `ResetApplicationAtExit` | 50.00% | Work in progress |
| `0x0049a7e0` | `GetApplicationHeapReserve` | 100.00% | Verified exact |
| `0x0049a7f0` | `GetApplicationIconResourceId` | 100.00% | Verified exact |
| `0x0049a800` | `ShowApplicationSettingsDialog` | 100.00% | Verified exact |
| `0x0049a820` | `ApplicationSettingsDialogProc` | 10.91% | Work in progress |
| `0x0049ada0` | `GetFileStorageMode` | 100.00% | Verified exact |
| `0x0049adc0` | `GetProfilerConfiguration` | 88.89% | Work in progress |
| `0x0049b5d0` | `Application::LoadDisplayConfig` | 66.67% | Work in progress |
| `0x0049b780` | `Application::SaveDisplayConfig` | 94.48% | Work in progress |
| `0x004a2130` | `CharBuffer24::CharBuffer24(char const *)` | 100.00% | Verified exact |
| `0x004a2170` | `CharBuffer24::CharBuffer24(void)` | 100.00% | Verified exact |
| `0x004a21b0` | `WinFile::WinFile` | 100.00% | Verified exact |
| `0x004a21f0` | `WinFile::Open` | 26.79% | Work in progress |
| `0x004a2290` | `WinFile::Read` | 95.24% | Reviewed near match |
| `0x004a22b0` | `WinFile::CloseManaged` | 100.00% | Verified exact |
| `0x004a22c0` | `WinFile::GetManagedSize` | 100.00% | Verified exact |
| `0x004a22d0` | `WinFile::~WinFile` | 100.00% | Verified exact |
| `0x004a22e0` | `WinFile::SeekManagedCurrent` | 100.00% | Verified exact |
| `0x004a2300` | `ManagedPathExists` | 100.00% | Verified exact |
| `0x004a2320` | `CharBuffer128::CharBuffer128(char const *)` | 100.00% | Verified exact |
| `0x004a2360` | `CharBuffer128::CharBuffer128(void)` | 100.00% | Verified exact |
| `0x004a2380` | `CharBuffer128::AppendTilde` | 100.00% | Verified exact |
| `0x004a23c0` | `CharBuffer128::FindAfterDot` | 100.00% | Verified exact |
| `0x004a23e0` | `CharBuffer128::SetText` | 100.00% | Verified exact |
| `0x004a2420` | `CharBuffer128::CopyBasenameTo` | 100.00% | Verified exact |
| `0x004a2460` | `CharBuffer128::Uppercase` | 100.00% | Verified exact |
| `0x004a2480` | `CharBuffer128::CopyPathWithSeparatorTo` | 100.00% | Verified exact |
| `0x004a2500` | `GetFileManager` | 83.33% | Work in progress |
| `0x004a2560` | `FileManager::FileManager` | 100.00% | Verified exact |
| `0x004a25c0` | `FileManager::Initialize` | 8.12% | Work in progress |
| `0x004a2810` | `FileManager::TrackOpen` | 3.45% | Work in progress |
| `0x004a2a30` | `FileManager::FindArchiveEntry` | 24.80% | Work in progress |
| `0x004a2ba0` | `FileManager::Read` | 100.00% | Work in progress |
| `0x004a2c20` | `FileManager::Close` | 100.00% | Verified exact |
| `0x004a2c40` | `FileManager::GetSize` | 100.00% | Verified exact |
| `0x004a2c60` | `FileManager::Exists` | 62.30% | Work in progress |
| `0x004a2d20` | `FileManager::SeekCurrent` | 79.49% | Work in progress |
| `0x004a2d80` | `FileManager::SetFlags` | 100.00% | Verified exact |
| `0x004a2dd0` | `GetGameManager` | 93.33% | Work in progress |
| `0x004a2e20` | `GameObject::GameObject` | 83.64% | Work in progress |
| `0x004a2ed0` | `GameObject::~GameObject` | 66.67% | Work in progress |
| `0x004a2f10` | `GameObject::Initialize` | 42.34% | Work in progress |
| `0x004a2fe0` | `GameObject::Remove` | 80.00% | Work in progress |
| `0x004a3010` | `GameObject::ApplyChildTransform` | 69.57% | Work in progress |
| `0x004a3040` | `GameObject::SetParent` | 33.77% | Work in progress |
| `0x004a3090` | `GameObject::ClearParent` | 42.25% | Work in progress |
| `0x004a30e0` | `GameObject::UpdateRegion` | 74.42% | Work in progress |
| `0x004a3150` | `GameObjectTemplate::NormalizeDevelopmentFlags` | 66.67% | Work in progress |
| `0x004a3170` | `GameManager::ResizeObjectRegistry` | 23.75% | Work in progress |
| `0x004a3290` | `GameManager::RegisterObject` | 45.31% | Work in progress |
| `0x004a3350` | `GameManager::UnregisterObject` | 72.22% | Work in progress |
| `0x004a33d0` | `GameManager::SortObjectsByParent` | 28.78% | Work in progress |
| `0x004a34a0` | `GameManager::RebuildObjectWorkLists` | 35.38% | Work in progress |
| `0x004a3580` | `GameManager::InitializeRegisteredObjects` | 85.71% | Work in progress |
| `0x004a36c0` | `GameManager::UpdateObjects` | 47.27% | Work in progress |
| `0x004a3710` | `GameManager::RemoveAllObjects` | 23.16% | Work in progress |
| `0x004a3780` | `FindRegisteredGameObjectByName` | 37.11% | Work in progress |
| `0x004a37f0` | `FindGameObjectByName` | 86.96% | Work in progress |
| `0x004a3810` | `SimulationScheduler::Reset` | 72.73% | Work in progress |
| `0x004a3840` | `SimulationScheduler::NeedsSimulationStep` | 66.46% | Work in progress |
| `0x004a3a20` | `IsGameObjectFlagSelected` | 64.52% | Work in progress |
| `0x004a3aa0` | `AdvanceGameFrame` | 54.72% | Work in progress |
| `0x004a3ca0` | `ShutdownGameManager` | 100.00% | Verified exact |
| `0x004a3ed0` | `GetMemoryHeap` | 56.00% | Work in progress |
| `0x004a3f20` | `DestroyMemoryHeapAtExit` | 100.00% | Verified exact |
| `0x004a3f30` | `MemoryHeap::EnterOperation` | 85.71% | Work in progress |
| `0x004a3f60` | `MemoryHeap::LeaveOperation` | 100.00% | Verified exact |
| `0x004a3f80` | `MemoryHeap::MemoryHeap` | 82.54% | Work in progress |
| `0x004a4040` | `MemoryHeap::SetUnknownFlag` | 100.00% | Verified exact |
| `0x004a4050` | `MemoryHeap::Allocate` | 79.17% | Work in progress |
| `0x004a40d0` | `MemoryHeap::AllocateFromEnd` | 79.17% | Work in progress |
| `0x004a4150` | `MemoryHeap::Free` | 100.00% | Verified exact |
| `0x004a4190` | `AllocateHeapMemory` | 100.00% | Verified exact |
| `0x004a41b0` | `FreeHeapMemory` | 100.00% | Verified exact |
| `0x004a41d0` | `MemoryHeap::AllocateBlock` | 67.84% | Work in progress |
| `0x004a4350` | `MemoryHeap::FreeBlock` | 79.67% | Work in progress |
| `0x004a4480` | `MemoryHeap::UpdateCursor` | 95.83% | Reviewed near match |
| `0x004a44c0` | `MemoryHeap::FindForward` | 73.68% | Work in progress |
| `0x004a44f0` | `MemoryHeap::FindBackward` | 100.00% | Verified exact |
| `0x004a4510` | `MemoryHeap::InitializeBlock` | 100.00% | Verified exact |
| `0x004a4570` | `WinFile::OpenPath` | 80.31% | Work in progress |
| `0x004a4620` | `WinFile::CreateAlways` | 100.00% | Verified exact |
| `0x004a46a0` | `WinFile::OpenOrCreate` | 100.00% | Verified exact |
| `0x004a4720` | `WinFile::SeekEnd` | 100.00% | Verified exact |
| `0x004a4740` | `WinFile::SeekBounded` | 100.00% | Verified exact |
| `0x004a4790` | `WinFile::SeekCurrent` | 100.00% | Verified exact |
| `0x004a47b0` | `WinFile::OpenAppend` | 100.00% | Verified exact |
| `0x004a4820` | `GtcBlockCache::GtcBlockCache` | 63.79% | Work in progress |
| `0x004a48e0` | `GtcBlockCache::~GtcBlockCache` | 76.19% | Work in progress |
| `0x004a4930` | `GtcBlockCache::LoadBlock` | 36.51% | Work in progress |
| `0x004a49f0` | `ReadCompressedData` | 46.85% | Work in progress |
| `0x004a4a90` | `WinFile::ReadAt` | 55.00% | Work in progress |
| `0x004a4ac0` | `WinFile::ReadExact` | 100.00% | Verified exact |
| `0x004a4b00` | `WinFile::WriteExact` | 100.00% | Verified exact |
| `0x004a4b40` | `WinFile::GetSize` | 100.00% | Verified exact |
| `0x004a4b60` | `WinFile::Close` | 100.00% | Verified exact |
| `0x004a4b80` | `PathExists` | 100.00% | Verified exact |
| `0x004a4bc0` | `GetPathDriveKind` | 35.71% | Work in progress |
| `0x004a4c80` | `Vector3::Normalize` | 100.00% | Verified exact |
| `0x004a4ce0` | `Vector3::FUN_004a4ce0` | 100.00% | Verified exact |
| `0x004a4d30` | `Matrix3::FUN_004a4d30` | 25.71% | Work in progress |
| `0x004a4e00` | `Vector3::FUN_004a4e00` | 100.00% | Verified exact |
| `0x004a4e50` | `Vector3::FUN_004a4e50` | 100.00% | Verified exact |
| `0x004a4ea0` | `Matrix3::FUN_004a4ea0` | 100.00% | Verified exact |
| `0x004a4f00` | `Matrix3::FUN_004a4f00` | 100.00% | Verified exact |
| `0x004a4f60` | `Matrix3::FUN_004a4f60` | 100.00% | Verified exact |
| `0x004a5040` | `Matrix3::FUN_004a5040` | 100.00% | Verified exact |
| `0x004a5080` | `Matrix3::FUN_004a5080` | 42.22% | Work in progress |
| `0x004a5110` | `Matrix3::FUN_004a5110` | 100.00% | Verified exact |
| `0x004a5150` | `Matrix3::FUN_004a5150` | 77.22% | Work in progress |
| `0x004a5250` | `Matrix3::FUN_004a5250` | 61.90% | Work in progress |
| `0x004a52c0` | `Matrix3::FUN_004a52c0` | 100.00% | Verified exact |
| `0x004a5310` | `Quaternion::Normalize` | 85.37% | Work in progress |
| `0x004a5380` | `PackedQuaternion::Pack` | 34.23% | Work in progress |
| `0x004a5440` | `Quaternion::Decode` | 83.87% | Work in progress |
| `0x004a5780` | `Quaternion::Interpolate` | 100.00% | Verified exact |
| `0x004a5dc0` | `RoundUpToPowerOfTwo` | 100.00% | Verified exact |
| `0x004a5df0` | `CountTrailingZeroBits` | 100.00% | Verified exact |
| `0x004a5e10` | `BuildSineTable` | 100.00% | Verified exact |
| `0x004a5e90` | `InitializeSimulationFrameRate` | 53.33% | Work in progress |
| `0x004a5ec0` | `InitializeSimulationStepSeconds` | 100.00% | Verified exact |
| `0x004a5ef0` | `InitializeSimulationStepMilliseconds` | 100.00% | Verified exact |
| `0x004a5f50` | `InitializeMaximumAngularSpeed` | 33.33% | Work in progress |
| `0x004a5f80` | `InitializeMaximumAngularSpeedSquared` | 100.00% | Verified exact |
| `0x004a5fb0` | `InitializeMaximumLinearSpeedSquared` | 33.33% | Work in progress |
| `0x004a6190` | `ResetGameObjectInitializationValue` | 63.16% | Work in progress |
| `0x004a6480` | `PhysicalLifecycleState::RebuildInverseInertiaTensor` | 100.00% | Verified exact |
| `0x004a64c0` | `PhysicalLifecycleState::UpdateAndClampVelocities` | 52.25% | Work in progress |
| `0x004a6a60` | `PhysicalLifecycleState::UpdateSleepState` | 77.38% | Work in progress |
| `0x004a6bb0` | `SetLifecycleParent` | 65.17% | Work in progress |
| `0x004a6c30` | `ClearLifecycleParent` | 51.52% | Work in progress |
| `0x004a6ec0` | `PhysicalLifecycleState::GetWorldParentOffset` | 87.80% | Work in progress |
| `0x004a6f00` | `PhysicalLifecycleState::GetParentVelocityAtOffset` | 84.62% | Work in progress |
| `0x004a74b0` | `IsLookupPointWithinRadius` | 45.45% | Work in progress |
| `0x004a7510` | `ApplyLookupResourceState` | 18.70% | Work in progress |
| `0x004a7cc0` | `?ReleaseRendererDrawPacket@@YIXPAURendererDrawPacket@?%Z:\home\ilya\work\lr2_decomp\racers2\LEGO Racers 2\include\applicationdispatch.h182315510@@PAX@Z` | 66.67% | Work in progress |
| `0x004a9d20` | `GetLookupResourceOrigin` | 91.30% | Work in progress |
| `0x004a9d90` | `GameObjectChildGeometry::SetPosition` | 16.00% | Work in progress |
| `0x004a9ff0` | `GameObjectChildGeometry::SetRotation` | 0.00% | Work in progress |
| `0x004aa3c0` | `SetRenderProjectionRatio` | 100.00% | Work in progress |
| `0x004aa540` | `GameObject::IsVisible` | 41.27% | Work in progress |
| `0x004aa730` | `IsPointWithinGameObjectView` | 36.36% | Work in progress |
| `0x004aa790` | `GameObject::DrawChild` | 40.00% | Work in progress |
| `0x004aa7b0` | `GameManager::UpdateComponents` | 21.52% | Work in progress |
| `0x004aa8e0` | `GameManager::RenderFrame` | 14.09% | Work in progress |
| `0x004ab2a0` | `ReportFatalError` | 85.25% | Work in progress |
| `0x004ab3b0` | `PrintDiagnostic` | 79.41% | Work in progress |
| `0x004ab560` | `RendererVertexBufferLegacyConditionalFree` | 75.86% | Work in progress |
| `0x004ab590` | `RendererVertexBuffer::GetDeviceBuffer` | 100.00% | Verified exact |
| `0x004ab5a0` | `RendererVertexBuffer::GetAttributes` | 100.00% | Verified exact |
| `0x004ab5b0` | `RendererVertexBuffer::GetCount` | 100.00% | Verified exact |
| `0x004ab5c0` | `RendererVertexBuffer::GetTextureCount` | 100.00% | Verified exact |
| `0x004ab5d0` | `RendererVertexBuffer::GetVertex` | 100.00% | Verified exact |
| `0x004ab5e0` | `RendererVertexBuffer::GetPositionOffset` | 100.00% | Verified exact |
| `0x004ab5f0` | `RendererVertexBuffer::GetNormalOffset` | 100.00% | Verified exact |
| `0x004ab600` | `RendererVertexBuffer::GetTextureOffset` | 100.00% | Verified exact |
| `0x004ab610` | `RendererVertexBuffer::SetCursor` | 100.00% | Verified exact |
| `0x004ab630` | `RendererVertexBuffer::AdvanceCursor` | 100.00% | Verified exact |
| `0x004ab640` | `RendererVertexBuffer::GetCurrentPosition` | 100.00% | Verified exact |
| `0x004ab650` | `RendererVertexBuffer::GetPosition` | 100.00% | Verified exact |
| `0x004ab670` | `RendererVertexBuffer::GetCurrentNormal` | 100.00% | Verified exact |
| `0x004ab680` | `RendererVertexBuffer::GetNormal` | 100.00% | Verified exact |
| `0x004ab6a0` | `RendererVertexBuffer::GetCurrentColor` | 100.00% | Verified exact |
| `0x004ab6b0` | `RendererVertexBuffer::GetColor` | 100.00% | Verified exact |
| `0x004ab6d0` | `RendererVertexBuffer::GetCurrentTextureCoordinate` | 66.67% | Work in progress |
| `0x004ab6f0` | `RendererVertexBuffer::GetTextureCoordinate` | 77.78% | Work in progress |
| `0x004ab710` | `RendererVertexBuffer::SetCurrentPosition` | 100.00% | Verified exact |
| `0x004ab730` | `RendererVertexBuffer::SetPosition` | 100.00% | Verified exact |
| `0x004ab760` | `RendererVertexBuffer::SetCurrentNormal` | 100.00% | Verified exact |
| `0x004ab780` | `RendererVertexBuffer::SetNormal` | 100.00% | Verified exact |
| `0x004ab7b0` | `RendererVertexBuffer::SetCurrentColor` | 100.00% | Verified exact |
| `0x004ab7d0` | `RendererVertexBuffer::SetColor` | 100.00% | Verified exact |
| `0x004ab7f0` | `RendererVertexBuffer::SetCurrentTextureCoordinate` | 43.48% | Work in progress |
| `0x004ab820` | `RendererVertexBuffer::SetTextureCoordinate` | 27.59% | Work in progress |
| `0x004ab850` | `RendererIndexBuffer::GetDeviceBuffer` | 100.00% | Verified exact |
| `0x004ab860` | `RendererIndexBuffer::Release` | 100.00% | Verified exact |
| `0x004ab890` | `RendererIndexBuffer::Lock` | 100.00% | Verified exact |
| `0x004ab900` | `RendererIndexBuffer::Unlock` | 64.90% | Work in progress |
| `0x004ab9c0` | `RendererIndexBuffer::SetFlag4` | 100.00% | Verified exact |
| `0x004ab9e0` | `DisplayRenderer::DisplayRenderer` | 19.73% | Work in progress |
| `0x004abdb0` | `RendererLookupTableState::RendererLookupTableState` | 100.00% | Verified exact |
| `0x004abdd0` | `DisplayRenderer::RejectUnknownDrawRequest` | 100.00% | Verified exact |
| `0x004abde0` | `RendererVertexBuffer::RendererVertexBuffer` | 61.11% | Work in progress |
| `0x004abe30` | `RendererIndexBuffer::RendererIndexBuffer` | 90.00% | Work in progress |
| `0x004abe50` | `RendererTextureRecord::RendererTextureRecord` | 90.00% | Work in progress |
| `0x004abe80` | `RendererTextureRecord::GetFlag0x90` | 100.00% | Verified exact |
| `0x004abe90` | `RendererZeroPair::RendererZeroPair` | 100.00% | Verified exact |
| `0x004abea0` | `RendererArrayMarker::RendererArrayMarker` | 100.00% | Verified exact |
| `0x004abeb0` | `RendererSubrecordGroup::RendererSubrecordGroup` | 100.00% | Verified exact |
| `0x004abef0` | `DisplayRenderer::GetSelectedLightDirection` | 100.00% | Verified exact |
| `0x004abf00` | `DisplayRenderer::UpdateTransformCache` | 9.30% | Work in progress |
| `0x004ac030` | `DisplayRenderer::HasPreferredDepthFormat` | 100.00% | Verified exact |
| `0x004ac040` | `DisplayRenderer::GetTextureRecord` | 100.00% | Verified exact |
| `0x004ac060` | `DisplayRenderer::GetStageBlend` | 100.00% | Verified exact |
| `0x004ac070` | `DisplayRenderer::SetStageBlend` | 100.00% | Verified exact |
| `0x004ac080` | `DisplayRenderer::GetTimingMetrics` | 100.00% | Verified exact |
| `0x004ac0a0` | `DisplayRenderer::GetMaximumTextureDimension` | 24.00% | Work in progress |
| `0x004ac0d0` | `DisplayRenderer::GetQuadVertexBuffer` | 100.00% | Verified exact |
| `0x004ac0e0` | `DisplayRenderer::SupportsUnknownDrawFeature` | 100.00% | Verified exact |
| `0x004ac0f0` | `DisplayRenderer::SetClockwiseCulling` | 100.00% | Verified exact |
| `0x004ac100` | `DisplayRenderer::SetUnknownDrawPolicy` | 100.00% | Verified exact |
| `0x004ac110` | `DisplayRenderer::GetTextureQuality` | 100.00% | Verified exact |
| `0x004ac120` | `DisplayRenderer::UsesHardwareVertexProcessing` | 100.00% | Verified exact |
| `0x004ac130` | `DisplayRenderer::Initialize` | 22.87% | Work in progress |
| `0x004acd00` | `DisplayRenderer::ConfigureNvidiaWorkaround` | 12.61% | Work in progress |
| `0x004acfb0` | `DisplayRenderer::CreateVertexBuffers` | 95.33% | Reviewed near match |
| `0x004ad060` | `DisplayRenderer::Shutdown` | 87.41% | Work in progress |
| `0x004ad130` | `GetChannelMaskBitCount` | 14.56% | Work in progress |
| `0x004ad2c0` | `DisplayRenderer::EnumerateSupportedAdapters` | 33.33% | Work in progress |
| `0x004ad540` | `GetDisplayFormatBitDepth` | 29.17% | Work in progress |
| `0x004ad5a0` | `DisplayRenderer::GetSelectedAdapterModeCount` | 100.00% | Verified exact |
| `0x004ad5c0` | `DisplayRenderer::GetSelectedAdapterMode` | 16.47% | Work in progress |
| `0x004ad640` | `DisplayRenderer::ReconfigureDisplayMode` | 12.84% | Work in progress |
| `0x004ada60` | `DisplayRenderer::FindBestDisplayMode` | 22.32% | Work in progress |
| `0x004adba0` | `DisplayRenderer::InitializeDefaultRenderStates` | 60.53% | Work in progress |
| `0x004adfe0` | `DisplayRenderer::SetCachedRenderState` | 100.00% | Verified exact |
| `0x004ae010` | `DisplayRenderer::ResetTextureStages` | 30.95% | Work in progress |
| `0x004ae310` | `DisplayRenderer::SetCachedTextureStageState` | 43.48% | Work in progress |
| `0x004ae360` | `DisplayRenderer::AddViewport` | 55.88% | Work in progress |
| `0x004ae3d0` | `DisplayRenderer::UpdateViewport` | 100.00% | Verified exact |
| `0x004ae430` | `DisplayRenderer::SelectViewport` | 100.00% | Verified exact |
| `0x004ae480` | `DisplayRenderer::SetProjectionFromViewport` | 20.51% | Work in progress |
| `0x004ae5a0` | `DisplayRenderer::ReleaseViewport` | 100.00% | Verified exact |
| `0x004ae5c0` | `DisplayRenderer::SetMaterialColor` | 17.48% | Work in progress |
| `0x004ae660` | `PixelFormatDescription::Initialize` | 69.44% | Work in progress |
| `0x004ae9d0` | `GetChannelMaskLoss` | 11.96% | Work in progress |
| `0x004aeb30` | `DisplayRenderer::SupportsCubeTextures` | 100.00% | Verified exact |
| `0x004aeb50` | `DisplayRenderer::GetDotProductStageFlag` | 38.10% | Work in progress |
| `0x004aeb70` | `DisplayRenderer::SupportsExtendedTextureStages` | 81.82% | Work in progress |
| `0x004aebb0` | `DisplayRenderer::SelectDepthStencilFormat` | 60.38% | Work in progress |
| `0x004aec60` | `DisplayRenderer::GetDeviceMemoryMegabytes` | 100.00% | Verified exact |
| `0x004aec70` | `DisplayRenderer::SupportsSharedMemoryMultitexturing` | 82.76% | Work in progress |
| `0x004aed00` | `CheckDirectInputResult` | 100.00% | Verified exact |
| `0x004aed30` | `InputDeviceService::Initialize` | 84.53% | Work in progress |
| `0x004aefe0` | `InputThreeArgumentNoOp` | 100.00% | Verified exact |
| `0x004aeff0` | `InputZeroAxis` | 100.00% | Verified exact |
| `0x004af000` | `InputDeviceService::PollDevices` | 83.33% | Work in progress |
| `0x004af050` | `InputDeviceService::Shutdown` | 91.53% | Work in progress |
| `0x004af090` | `InputDeviceService::RegisterDevice` | 41.38% | Work in progress |
| `0x004af130` | `InputDeviceService::AcquireAvailableDevices` | 100.00% | Verified exact |
| `0x004af170` | `InputDeviceService::RefreshDevices` | 86.27% | Work in progress |
| `0x004af1a0` | `InputDeviceService::FindDeviceByType` | 57.58% | Work in progress |
| `0x004af1e0` | `InputDeviceRecord::LoadDeviceInfo` | 57.43% | Work in progress |
| `0x004af2b0` | `GetAudioVectorLength` | 100.00% | Verified exact |
| `0x004af2f0` | `GetAudioService` | 64.10% | Work in progress |
| `0x004af360` | `DestroyAudioService` | 73.68% | Work in progress |
| `0x004af380` | `AudioService::AudioService` | 76.32% | Work in progress |
| `0x004af4e0` | `AudioService::Initialize` | 65.16% | Work in progress |
| `0x004af6c0` | `AudioService::GetQualityMode` | 100.00% | Work in progress |
| `0x004af6d0` | `AudioService::SetQualityMode` | 66.67% | Work in progress |
| `0x004af710` | `AudioService::SetVolumes` | 36.07% | Work in progress |
| `0x004af790` | `AudioService::GetVolumes` | 100.00% | Work in progress |
| `0x004af7b0` | `SetAudioParameters` | 66.67% | Work in progress |
| `0x004af7e0` | `AudioService::SetMusicEnabled` | 72.00% | Work in progress |
| `0x004af810` | `AudioService::Update` | 76.67% | Work in progress |
| `0x004af860` | `UpdateAudioDeviceBuffers` | 64.10% | Work in progress |
| `0x004af8d0` | `UpdateAudioSourceOutputs` | 36.36% | Work in progress |
| `0x004af980` | `ProcessAudioSourceFrames` | 4.61% | Work in progress |
| `0x004aff40` | `AddAudioSource` | 23.33% | Work in progress |
| `0x004b01e0` | `ReleaseAudioSourceRecord` | 72.73% | Work in progress |
| `0x004b01f0` | `RemoveAudioSource` | 73.17% | Work in progress |
| `0x004b0220` | `FindAudioSource` | 15.79% | Work in progress |
| `0x004b0290` | `ValidateAudioSource` | 63.41% | Work in progress |
| `0x004b02d0` | `InitializeAudioSources` | 64.52% | Work in progress |
| `0x004b0320` | `FindAvailableAudioSourceRecord` | 90.91% | Work in progress |
| `0x004b0340` | `ConstructAudioSourceRecord` | 45.71% | Work in progress |
| `0x004b0370` | `DeleteBaseAudioSourceRecord` | 100.00% | Work in progress |
| `0x004b0390` | `DeleteAudioSourceRecord` | 88.00% | Work in progress |
| `0x004b03b0` | `InitializeAudioSourceRecordFields` | 10.20% | Work in progress |
| `0x004b04c0` | `DisposeAudioSourceRecord` | 82.35% | Work in progress |
| `0x004b04e0` | `ReleaseAudioSourceRecordResources` | 55.56% | Work in progress |
| `0x004b0500` | `ReleaseAudioSourcePlayback` | 46.51% | Work in progress |
| `0x004b0550` | `GetAudioSourceRecordIndex` | 100.00% | Work in progress |
| `0x004b0560` | `SetAudioSourceRecordValues0x10` | 100.00% | Work in progress |
| `0x004b0580` | `SetAudioSourceRecordValues0x04` | 100.00% | Work in progress |
| `0x004b05a0` | `SetAudioSourceRecordValues0x1c` | 100.00% | Work in progress |
| `0x004b05c0` | `SetAudioSourceRecordFalloff` | 100.00% | Work in progress |
| `0x004b05d0` | `SetAudioSourceRecordFlagsLow` | 100.00% | Work in progress |
| `0x004b05f0` | `SetAudioSourceRecordFlagsHigh` | 100.00% | Work in progress |
| `0x004b0610` | `SetAudioSourceRecordValues0x90` | 100.00% | Work in progress |
| `0x004b0630` | `SetAudioSourceRecordValues0x98` | 100.00% | Work in progress |
| `0x004b0650` | `SetAudioSourceRecordValues0xa0` | 100.00% | Work in progress |
| `0x004b0670` | `SetAudioSourceRecordGains` | 52.31% | Work in progress |
| `0x004b0700` | `SetAudioSourceRecordMaximumGain` | 58.82% | Work in progress |
| `0x004b0750` | `ResetAudioSourcePlaybackState` | 100.00% | Work in progress |
| `0x004b0780` | `ClearAudioSourceFlag` | 100.00% | Work in progress |
| `0x004b0790` | `SetAudioSourceEnabled` | 100.00% | Work in progress |
| `0x004b07c0` | `InitializeAudioChannels` | 81.48% | Work in progress |
| `0x004b08a0` | `FindOrCreateAudioChannel` | 51.06% | Work in progress |
| `0x004b0a20` | `ConstructAudioChannelRecord` | 0.00% | Work in progress |
| `0x004b0a40` | `InitializeAudioChannelRecord` | 15.38% | Work in progress |
| `0x004b0a90` | `DestroyAudioChannelRecordResources` | 21.05% | Work in progress |
| `0x004b0ad0` | `LoadAudioChannelFile` | 64.62% | Work in progress |
| `0x004b0bd0` | `ReleaseAudioChannelRecordBuffer` | 100.00% | Work in progress |
| `0x004b0c20` | `AudioService::AddListener` | 35.29% | Work in progress |
| `0x004b0c60` | `AudioService::RemoveListener` | 94.12% | Work in progress |
| `0x004b0ca0` | `AudioService::SetListenerField0x04` | 100.00% | Work in progress |
| `0x004b0cc0` | `AudioService::SetListenerField0x30` | 100.00% | Work in progress |
| `0x004b0ce0` | `AudioService::SetListenerGains` | 53.33% | Work in progress |
| `0x004b0d80` | `InitializeAudioDevices` | 68.97% | Work in progress |
| `0x004b0da0` | `AudioService::FindAvailableListener` | 19.35% | Work in progress |
| `0x004b0dc0` | `AudioService::FindListenerById` | 8.16% | Work in progress |
| `0x004b0df0` | `FindNearestAudioListener(class AudioService *, void *, class Vector3 const *, class Vector3 *)` | 69.70% | Work in progress |
| `0x004b0ed0` | `FindNearestAudioListener(2)` | 57.14% | Work in progress |
| `0x004b0fa0` | `ConstructAudioDeviceRecord` | 33.33% | Work in progress |
| `0x004b0fc0` | `InitializeAudioDeviceRecord` | 14.29% | Work in progress |
| `0x004b1020` | `AudioService::Shutdown` | 81.22% | Work in progress |
| `0x004b1150` | `AudioService::Restart` | 48.43% | Work in progress |
| `0x004b13d0` | `GetAudioServiceShutdownStatus` | 100.00% | Verified exact |
| `0x004b2960` | `GetSystemClock` | 92.86% | Work in progress |
| `0x004b29b0` | `SystemClock::Initialize` | 37.04% | Work in progress |
| `0x004b2b50` | `ReturnZeroClockValue` | 100.00% | Verified exact |
| `0x004b2b60` | `GetClockCalibration` | 100.00% | Verified exact |
| `0x004b2b70` | `ResetClock` | 70.00% | Work in progress |
| `0x004b2b90` | `PushCycleCounter` | 7.41% | Work in progress |
| `0x004b2bc0` | `PopCycleCounter` | 5.71% | Work in progress |
| `0x004b2c00` | `ReadSystemClockCycles` | 80.95% | Work in progress |
| `0x004b2c40` | `QueryClockTime` | 80.00% | Work in progress |
| `0x004b2cc0` | `UpdateElapsedTime` | 100.00% | Verified exact |
| `0x004b2d30` | `InitializeGlobalOverlayManagerAndRegisterExit` | 100.00% | Work in progress |
| `0x004b2d40` | `InitializeGlobalOverlayManager` | 42.67% | Work in progress |
| `0x004b2da0` | `RegisterGlobalOverlayManagerExit` | 100.00% | Work in progress |
| `0x004b2db0` | `DestroyGlobalOverlayManager` | 7.69% | Work in progress |
| `0x004b2de0` | `GlobalOverlaySprite::Initialize` | 100.00% | Verified exact |
| `0x004b2e00` | `GlobalOverlaySprite::Release` | 87.50% | Work in progress |
| `0x004b2e20` | `GlobalOverlaySprite::SetVisible` | 100.00% | Verified exact |
| `0x004b2e40` | `GlobalOverlaySprite::SetPosition` | 61.05% | Work in progress |
| `0x004b2f00` | `GlobalOverlaySprite::Translate` | 71.43% | Work in progress |
| `0x004b2f30` | `GlobalOverlaySprite::SetSize` | 100.00% | Verified exact |
| `0x004b2fb0` | `GlobalOverlaySprite::GetSize` | 100.00% | Verified exact |
| `0x004b2fd0` | `GlobalOverlaySprite::GetRectangle` | 100.00% | Verified exact |
| `0x004b3000` | `GlobalOverlaySprite::SetTextureRegion` | 100.00% | Verified exact |
| `0x004b3030` | `GlobalOverlaySprite::SetTexture` | 100.00% | Verified exact |
| `0x004b3040` | `GlobalOverlaySprite::SetColor` | 100.00% | Verified exact |
| `0x004b3050` | `GlobalOverlaySprite::SetAlpha` | 100.00% | Verified exact |
| `0x004b3060` | `GlobalOverlaySprite::SetType` | 100.00% | Verified exact |
| `0x004b3070` | `GlobalOverlaySprite::SetList` | 92.86% | Work in progress |
| `0x004b3090` | `GlobalOverlaySprite::SetClipRectangle` | 100.00% | Verified exact |
| `0x004b30c0` | `GlobalOverlayManager::GlobalOverlayManager` | 5.33% | Work in progress |
| `0x004b3330` | `GlobalOverlayManagerDestroy` | 83.33% | Work in progress |
| `0x004b3370` | `GlobalOverlayManagerUpdateRecords` | 83.72% | Work in progress |
| `0x004b33b0` | `GlobalOverlayManager::AllocateSprite` | 100.00% | Verified exact |
| `0x004b33f0` | `GlobalOverlayManager::MoveSprite` | 86.27% | Work in progress |
| `0x004b3470` | `GlobalOverlayManager::AllocateRecord` | 50.00% | Work in progress |
| `0x004b34b0` | `GlobalOverlayManager::MoveRecord` | 80.77% | Work in progress |
| `0x004b3540` | `RenderGlobalOverlayPass` | 21.35% | Work in progress |
| `0x004b3880` | `GlobalOverlayManager::SetLocalizedStrings` | 100.00% | Verified exact |
| `0x004b38b0` | `GlobalOverlayManager::SetGlyphReplacementManager` | 100.00% | Verified exact |
| `0x004b38c0` | `GlobalOverlayRecord::InitializeFromParameters` | 36.36% | Work in progress |
| `0x004b3920` | `GlobalOverlayRecord::Release` | 85.71% | Work in progress |
| `0x004b3980` | `GlobalOverlayRecord::InitializeText` | 43.40% | Work in progress |
| `0x004b3b10` | `GlobalOverlayRecord::InitializeTextByIndex` | 71.64% | Work in progress |
| `0x004b3b70` | `GlobalOverlayRecord::InitializeNarrowText` | 100.00% | Verified exact |
| `0x004b3d40` | `GlobalOverlayRecord::SetPosition` | 90.14% | Work in progress |
| `0x004b3dc0` | `GlobalOverlayRecord::Translate` | 75.00% | Work in progress |
| `0x004b3df0` | `GlobalOverlayRecord::SetColor` | 100.00% | Verified exact |
| `0x004b3e00` | `GlobalOverlayRecord::SetAlpha` | 100.00% | Verified exact |
| `0x004b3e10` | `GlobalOverlayRecord::SetList` | 92.86% | Work in progress |
| `0x004b3e40` | `GlobalOverlayRecord::SetFont` | 100.00% | Verified exact |
| `0x004b3ea0` | `GlobalOverlayRecord::GetEffectiveScale` | 100.00% | Verified exact |
| `0x004b3ec0` | `GlobalOverlayRecord::GetLineHeight` | 100.00% | Verified exact |
| `0x004b3ee0` | `GlobalOverlayRecord::SetClipRectangle` | 100.00% | Verified exact |
| `0x004b3f10` | `GlobalOverlayRecord::GetTextLength` | 100.00% | Verified exact |
| `0x004b3f30` | `GlobalOverlayRecord::GetRectangle` | 56.72% | Work in progress |
| `0x004b3fd0` | `DrawGlobalOverlayText` | 6.73% | Work in progress |
| `0x004b43d0` | `AdvanceGlobalOverlayText` | 55.12% | Work in progress |
| `0x004b44e0` | `MeasureGlobalOverlayText` | 6.17% | Work in progress |
| `0x004b4690` | `FUN_004b4690` | 100.00% | Verified exact |
| `0x004b46c0` | `CreateNativeWindow` | 90.72% | Work in progress |
| `0x004b4750` | `RegisterNativeWindowClass` | 32.00% | Work in progress |
| `0x004b4800` | `CreateNativeWindowEx` | 43.07% | Work in progress |
| `0x004b49e0` | `FUN_004b49e0` | 100.00% | Verified exact |
| `0x004b4a60` | `FUN_004b4a60` | 100.00% | Verified exact |
| `0x004b4a80` | `FUN_004b4a80` | 100.00% | Verified exact |
| `0x004b4b30` | `FUN_004b4b30` | 100.00% | Verified exact |
| `0x004b4b80` | `GetWindowWidth` | 100.00% | Verified exact |
| `0x004b4bb0` | `GetWindowHeight` | 100.00% | Verified exact |
| `0x004b4be0` | `GetWindowLeft` | 100.00% | Verified exact |
| `0x004b4c00` | `GetWindowTop` | 100.00% | Verified exact |
| `0x004b4c20` | `GetWindowOwner` | 100.00% | Verified exact |
| `0x004b4c30` | `GetWindowStyle` | 100.00% | Verified exact |
| `0x004b4c40` | `SetWindowWidth` | 100.00% | Verified exact |
| `0x004b4cd0` | `SetWindowHeight` | 100.00% | Verified exact |
| `0x004b4d60` | `ExpandChildWindowBounds` | 100.00% | Verified exact |
| `0x004b4de0` | `FitWindowToChildren` | 100.00% | Verified exact |
| `0x004b4e80` | `ScreenRectToClient` | 100.00% | Verified exact |
| `0x004b7550` | `LocalizedStringTable::LocalizedStringTable` | 100.00% | Verified exact |
| `0x004b75c0` | `LocalizedStringTable::Load` | 58.40% | Work in progress |
| `0x004b7740` | `LocalizedStringTable::Release` | 63.64% | Work in progress |
| `0x004b77b0` | `LocalizedStringTable::SelectLanguage` | 34.29% | Work in progress |
| `0x004b78c0` | `LocalizedStringTable::GetString` | 100.00% | Verified exact |
| `0x004b79c0` | `LocalizedStringTable::BuildLanguagePath` | 100.00% | Verified exact |
| `0x004b7a60` | `LocalizedStringTable::BuildSortedIndex` | 87.06% | Work in progress |
| `0x004b7b80` | `InputController::InputController` | 83.33% | Work in progress |
| `0x004b7c20` | `InputController::~InputController` | 100.00% | Verified exact |
| `0x004b7c50` | `InputControllerManager::InitializeTiming` | 94.12% | Work in progress |
| `0x004b7c70` | `InputControllerManager::ResetRecording` | 28.38% | Work in progress |
| `0x004b7d40` | `InputControllerManager::Shutdown` | 98.18% | Reviewed near match |
| `0x004b7d80` | `InputController::SetOwner` | 100.00% | Verified exact |
| `0x004b7da0` | `InputController::AppendBinding` | 100.00% | Verified exact |
| `0x004b7dd0` | `GetControllerInputDevice` | 89.66% | Work in progress |
| `0x004b7e30` | `InputController::GetBinding` | 100.00% | Verified exact |
| `0x004b7e40` | `InputController::GetRecordedInput` | 71.11% | Work in progress |
| `0x004b7eb0` | `InputController::GetCurrentInput` | 65.57% | Work in progress |
| `0x004b7f20` | `InputController::RecordInputRange` | 92.00% | Work in progress |
| `0x004b7f60` | `InputControllerManager::FindOrCreateController` | 48.23% | Work in progress |
| `0x004b8010` | `InputControllerManager::RebindController` | 53.33% | Work in progress |
| `0x004b8040` | `InputControllerManager::ConfigureController` | 53.24% | Work in progress |
| `0x004b8220` | `InputControllerManager::AppendControllerBinding` | 66.67% | Work in progress |
| `0x004b8460` | `InputControllerManager::GetTimingState` | 34.85% | Work in progress |
| `0x004b8520` | `InputControllerManager::GetElapsedTime` | 45.45% | Work in progress |
| `0x004b8570` | `InputControllerManager::GetCurrentFrame` | 100.00% | Verified exact |
| `0x004b85a0` | `InputControllerManager::PollInput` | 47.84% | Work in progress |
| `0x004b86f0` | `InputControllerManager::PollController` | 11.13% | Work in progress |
| `0x004b8f40` | `InputControllerManager::SetPaused` | 37.18% | Work in progress |
| `0x004b93e0` | `InitializeInputControllerManagerStorage` | 66.67% | Work in progress |
| `0x004b94c0` | `InputControllerManager::AcquirePollMutex` | 100.00% | Work in progress |
| `0x004b94e0` | `InputControllerManager::ReleasePollMutex` | 100.00% | Verified exact |
| `0x004b9500` | `InputControllerManager::InitializePolling` | 100.00% | Verified exact |
| `0x004b9560` | `InputControllerManager::StopPolling` | 69.84% | Work in progress |
| `0x004b95b0` | `ControllerPollTimerCallback` | 100.00% | Verified exact |
| `0x004b95e0` | `InitializeCollisionServices` | 34.25% | Work in progress |
| `0x004b96d0` | `DestroyCollisionServiceObject` | 100.00% | Work in progress |
| `0x004b96f0` | `CollisionContact::SwapParticipants` | 90.00% | Work in progress |
| `0x004b9810` | `CreateCollisionProxy` | 84.21% | Work in progress |
| `0x004b9850` | `AttachCollisionProxy` | 52.63% | Work in progress |
| `0x004b9870` | `DestroyCollisionProxy` | 80.00% | Work in progress |
| `0x004b9940` | `GameFloatRandom::Initialize` | 100.00% | Verified exact |
| `0x004b9980` | `GameIntegerRandom::Initialize` | 100.00% | Verified exact |
| `0x004b99b0` | `GetGameRandomState` | 95.83% | Reviewed near match |
| `0x004ba280` | `PrepareGameObjectForRendering` | 33.08% | Work in progress |
| `0x004ba370` | `GetGameObjectViewDepth` | 92.68% | Work in progress |
| `0x004ba3b0` | `UpdateGameObjectRenderGeometry` | 19.95% | Work in progress |
| `0x004bb1f0` | `SetScreenshotDirectory` | 100.00% | Verified exact |
| `0x004bb210` | `CaptureScreenshot` | 38.59% | Work in progress |
| `0x004bb670` | `InitializeGlobalPauseController` | 37.33% | Work in progress |
| `0x004bb6d0` | `RegisterGlobalPauseControllerExit` | 100.00% | Work in progress |
| `0x004bb6e0` | `DestroyGlobalPauseController` | 40.00% | Work in progress |
| `0x004bb6f0` | `GlobalPauseController::GlobalPauseController` | 68.18% | Work in progress |
| `0x004bb740` | `GlobalPauseController::SetObserver` | 100.00% | Work in progress |
| `0x004bb750` | `GlobalPauseController::ClearObserver` | 66.67% | Work in progress |
| `0x004bb7a0` | `GlobalPauseController::EnterPause` | 81.97% | Work in progress |
| `0x004bb7f0` | `GlobalPauseController::LeavePause` | 75.76% | Work in progress |
| `0x004bb850` | `GlobalPauseController::Update` | 45.36% | Work in progress |
| `0x004bb8f0` | `GlobalPauseController::SetDeadline` | 100.00% | Work in progress |
| `0x004bb910` | `GlobalPauseController::IsBeforeDeadline` | 60.71% | Work in progress |
| `0x004bd5f0` | `RendererLookupTableState::Initialize` | 51.97% | Work in progress |
| `0x004bd6d0` | `RendererLookupTableState::ReleaseStorage` | 66.67% | Work in progress |
| `0x004bd710` | `RendererLookupTableState::Clear` | 28.07% | Work in progress |
| `0x004bd770` | `RendererLookupTableState::Insert` | 60.00% | Work in progress |
| `0x004bd7c0` | `RendererLookupTableState::InsertOrdered` | 100.00% | Verified exact |
| `0x004bd830` | `RendererLookupTableState::InsertLinear` | 76.36% | Work in progress |
| `0x004bd870` | `RendererLookupTableState::FlushOrderedEntries` | 58.18% | Work in progress |
| `0x004bd8c0` | `RendererLookupTableState::FlushBucketedEntries` | 81.48% | Work in progress |
| `0x004bd900` | `RendererLookupTableState::FlushDeferredDrawResources` | 33.50% | Work in progress |
| `0x004bda20` | `DisplayRenderer::DrawLookupEntry` | 12.55% | Work in progress |
| `0x004be0a0` | `DisplayRenderer::InitializeResources` | 50.16% | Work in progress |
| `0x004be2f0` | `RendererResourceCacheEntry::RendererResourceCacheEntry` | 27.27% | Work in progress |
| `0x004be320` | `RendererAuxiliaryResourceRecord::InitializeHeader` | 100.00% | Verified exact |
| `0x004be330` | `DisplayRenderer::ShutdownResources` | 61.24% | Work in progress |
| `0x004be460` | `BuildAnimationHueColor` | 16.10% | Work in progress |
| `0x004bf150` | `DisplayRenderer::SelectLights` | 25.63% | Work in progress |
| `0x004bf6e0` | `DisplayRenderer::ReapplyLights` | 66.21% | Work in progress |
| `0x004bf860` | `DisplayRenderer::GetMaximumDirectionalLightIntensity` | 42.72% | Work in progress |
| `0x004bf920` | `LoadMipSidecar` | 47.11% | Work in progress |
| `0x004bfa60` | `EnsureMipSidecar` | 42.22% | Work in progress |
| `0x004bfb50` | `DisplayRenderer::FindOrCreateTextureRecord` | 23.90% | Work in progress |
| `0x004bfce0` | `RendererTextureRecord::SetResourcePath` | 80.46% | Work in progress |
| `0x004bfd70` | `RendererTextureRecord::Load` | 29.51% | Work in progress |
| `0x004bffb0` | `GetTexturePathHash` | 100.00% | Verified exact |
| `0x004bffd0` | `RendererTextureRecord::MatchesResource` | 100.00% | Verified exact |
| `0x004c0050` | `ReleaseRendererTextureResource` | 11.76% | Work in progress |
| `0x004c0070` | `RendererResourceCacheEntry::SetPath` | 96.88% | Reviewed near match |
| `0x004c00c0` | `RendererResourceCacheEntry::LoadAnimation` | 31.83% | Work in progress |
| `0x004c03c0` | `RendererResourceCacheEntry::AdvanceFrame` | 20.69% | Work in progress |
| `0x004c0450` | `RendererResourceCacheEntry::ReleaseAnimationReferences` | 100.00% | Verified exact |
| `0x004c0510` | `RendererAuxiliaryResourceRecord::InitializeFrom` | 48.15% | Work in progress |
| `0x004c05e0` | `RendererAuxiliaryResourceRecord::Matches` | 47.93% | Work in progress |
| `0x004c0670` | `RendererAuxiliaryResourceRecord::SetCallback` | 61.54% | Work in progress |
| `0x004c06d0` | `DisplayRenderer::GetAuxiliaryResource` | 100.00% | Verified exact |
| `0x004c0770` | `DisplayRenderer::AcquireAuxiliaryResource` | 35.54% | Work in progress |
| `0x004c0910` | `DisplayRenderer::UpdateAnimatedResources` | 84.11% | Work in progress |
| `0x004c09a0` | `DisplayRenderer::FindOrAllocateTextureResource` | 44.59% | Work in progress |
| `0x004c0a60` | `DisplayRenderer::AddTextureReference` | 89.66% | Work in progress |
| `0x004c0ae0` | `DisplayRenderer::FindTextureReferenceById` | 44.44% | Work in progress |
| `0x004c0b30` | `DisplayRenderer::FindTextureReference` | 51.16% | Work in progress |
| `0x004c0b70` | `DisplayRenderer::CreateTextureResource` | 13.55% | Work in progress |
| `0x004c0ee0` | `DisplayRenderer::ReleaseTextureResourceReference` | 74.69% | Work in progress |
| `0x004c1050` | `DisplayRenderer::ReleaseAuxiliaryResource` | 45.65% | Work in progress |
| `0x004c10d0` | `RegisterResourceCallback` | 40.45% | Work in progress |
| `0x004c1150` | `UnregisterResourceCallback` | 18.02% | Work in progress |
| `0x004c11e0` | `RendererAuxiliaryResourceRecord::InitializeMaterial` | 19.77% | Work in progress |
| `0x004c3020` | `StopNativeGame` | 100.00% | Work in progress |
| `0x004c3040` | `RunGameInstance` | 67.34% | Work in progress |
| `0x004c32b0` | `FUN_004c32b0` | 100.00% | Verified exact |
| `0x004c32c0` | `GameWindowProc` | 85.19% | Work in progress |
| `0x004c33b0` | `FUN_004c33b0` | 100.00% | Verified exact |
| `0x004c33f0` | `HandlePrivateAudioMessage` | 34.78% | Work in progress |
| `0x004c3460` | `FindFirstNonemptyRow` | 100.00% | Verified exact |
| `0x004c3540` | `BitmapFont::Load` | 19.11% | Work in progress |
| `0x004c3d70` | `BitmapImage::BitmapImage` | 100.00% | Verified exact |
| `0x004c3db0` | `BitmapFont::GetGlyphMetrics` | 100.00% | Verified exact |
| `0x004c3e30` | `BitmapFont::GetTextAdvance` | 37.88% | Work in progress |
| `0x004c3ed0` | `BitmapFont::CopyFrom` | 63.33% | Work in progress |
| `0x004c3f40` | `Terrain::GetCellVertices` | 63.61% | Work in progress |
| `0x004c4140` | `Terrain::GetMinimumBounds` | 100.00% | Verified exact |
| `0x004c41a0` | `Terrain::GetMaximumBounds` | 100.00% | Verified exact |
| `0x004c4eb0` | `GlobalOverlayGlyphReplacementManagerView::RegisterIcon` | 100.00% | Verified exact |
| `0x004c4ed0` | `GlobalOverlayGlyphReplacementManagerView::SetIconTexture` | 100.00% | Verified exact |
| `0x004c4ef0` | `GlobalOverlayGlyphReplacementManagerView::GetIconTexture` | 100.00% | Verified exact |
| `0x004c4f00` | `GlobalOverlayGlyphReplacementManagerView::RegisterString` | 100.00% | Verified exact |
| `0x004c4f20` | `GlobalOverlayGlyphReplacementManagerView::SetStringSlot` | 100.00% | Verified exact |
| `0x004c4f30` | `CompareOverlayTextPrefix` | 57.14% | Work in progress |
| `0x004c4f70` | `HasGlobalOverlayGlyphReplacement` | 90.91% | Work in progress |
| `0x004c4fc0` | `ApplyGlobalOverlayGlyphReplacement` | 49.38% | Work in progress |
| `0x004c5030` | `MeasureGlobalOverlayGlyphReplacement` | 82.35% | Work in progress |
| `0x004c5090` | `GetGlobalOverlayGlyphReplacementAdvance` | 50.00% | Work in progress |
| `0x004c50b0` | `FindOverlayGlyphSequence` | 54.79% | Work in progress |
| `0x004c5110` | `ExpandOverlayGlyphSequence` | 42.86% | Work in progress |
| `0x004c6980` | `RenderInitializedResource` | 15.09% | Work in progress |
| `0x004c6fe0` | `RenderDeferredResource` | 20.66% | Work in progress |
| `0x004cb340` | `TerrainVertexGrid::SelectEdges` | 100.00% | Verified exact |
| `0x004cb530` | `UpdateTerrainLodMetrics` | 56.74% | Work in progress |
| `0x004cb630` | `Terrain::GetVertex` | 100.00% | Verified exact |
| `0x004cb680` | `Terrain::GetTile` | 100.00% | Verified exact |
| `0x004cd7c0` | `CachedConstraint::CachedConstraint` | 26.23% | Work in progress |
| `0x004cd820` | `CachedConstraint::IsInactive` | 70.59% | Work in progress |
| `0x004cd870` | `CachedConstraint::GetPositionDifference` | 100.00% | Verified exact |
| `0x004cd8c0` | `CachedConstraint::GetVelocityDifference` | 100.00% | Verified exact |
| `0x004cd910` | `CachedConstraint::ApplyForces` | 30.08% | Work in progress |
| `0x004cdb00` | `CachedConstraint::CopyContact` | 61.86% | Work in progress |
| `0x004cdc10` | `CachedConstraint::GetAccelerationDifference` | 100.00% | Verified exact |
| `0x004cdc60` | `CachedConstraint::ComputeNormalForce` | 79.61% | Work in progress |
| `0x004cdd00` | `CachedConstraint::RebuildEffectiveMassMatrix` | 100.00% | Verified exact |
| `0x004cdd70` | `ConstraintEndpoint::SetBinding` | 28.57% | Work in progress |
| `0x004cde00` | `ConstraintEndpoint::SetWorldBinding` | 42.86% | Work in progress |
| `0x004cde60` | `ConstraintEndpoint::GetWorldPosition` | 98.70% | Reviewed near match |
| `0x004cdec0` | `ConstraintEndpoint::UpdateWorldOffset` | 39.29% | Work in progress |
| `0x004cdf10` | `ConstraintEndpoint::GetPointVelocity` | 8.89% | Work in progress |
| `0x004cdff0` | `ConstraintEndpoint::UpdatePointAcceleration` | 6.63% | Work in progress |
| `0x004ce1c0` | `ConstraintEndpoint::GetPointAcceleration` | 55.76% | Work in progress |
| `0x004ce300` | `ConstraintEndpoint::GetEffectiveMassMatrix` | 53.33% | Work in progress |
| `0x004d0f60` | `SplinePath::FUN_004d0f60` | 48.34% | Work in progress |
| `0x004d11c0` | `SplinePath::FUN_004d11c0` | 47.13% | Work in progress |
| `0x004d1390` | `SplinePath::FUN_004d1390` | 47.00% | Work in progress |
| `0x004d15e0` | `SplineBasis::SplineBasis` | 100.00% | Verified exact |
| `0x004d1610` | `SplineBasis::FUN_004d1610` | 89.45% | Work in progress |
| `0x004d17d0` | `SplineBasis::FUN_004d17d0` | 100.00% | Verified exact |
| `0x004d1800` | `FUN_004d1800` | 65.28% | Work in progress |
| `0x004d1a20` | `SplinePath::FUN_004d1a20` | 100.00% | Verified exact |
| `0x004d1a70` | `SplinePath::FUN_004d1a70` | 100.00% | Verified exact |
| `0x004d1aa0` | `SplinePath::FUN_004d1aa0` | 39.73% | Work in progress |
| `0x004d1d90` | `SplinePath::FUN_004d1d90` | 48.32% | Work in progress |
| `0x004d3a60` | `UpdateTerrainRegionVisibility` | 9.56% | Work in progress |
| `0x004d3e10` | `FindTerrainRegionCellData` | 35.29% | Work in progress |
| `0x004d3e60` | `IsTerrainRegionRayCell` | 29.27% | Work in progress |
| `0x004d3f00` | `CollectTerrainRegionCandidates` | 43.97% | Work in progress |
| `0x004d40a0` | `TestGameObjectRegion` | 18.53% | Work in progress |
| `0x004d42e0` | `TerrainVisibilityContext::GetCellState` | 9.72% | Work in progress |
| `0x004d4520` | `ClassifyTerrainRegionCell` | 23.85% | Work in progress |
| `0x004d46a0` | `RebuildTerrainRegionVisibility` | 20.78% | Work in progress |
| `0x004d48e0` | `UpdateTerrainRegionCellNeighbors` | 11.51% | Work in progress |
| `0x004d5be0` | `Vector3::Cross` | 17.02% | Work in progress |
| `0x004d5cd0` | `BuildTerrainCellCorners` | 21.21% | Work in progress |
| `0x004d6050` | `IntersectTerrainQuad` | 100.00% | Verified exact |
| `0x004d60e0` | `VisitTerrainSegmentCell` | 100.00% | Verified exact |
| `0x004d6160` | `IntersectTerrainSurfaceSegment` | 90.41% | Work in progress |
| `0x004d7eb0` | `CollisionProxy::CollisionProxy` | 67.77% | Work in progress |
| `0x004d7fa0` | `CollisionProxy::~CollisionProxy` | 100.00% | Work in progress |
| `0x004d7ff0` | `CollisionProxy::SetTransform` | 76.92% | Work in progress |
| `0x004d8090` | `CollisionProxy::SetChildTransform` | 60.00% | Work in progress |
| `0x004d8110` | `CollisionProxy::DetachFromIndex` | 83.33% | Work in progress |
| `0x004d8150` | `CollisionProxy::AttachToIndex` | 95.00% | Work in progress |
| `0x004d8190` | `CollisionModelView::IntersectSegment` | 94.74% | Work in progress |
| `0x004d81b0` | `CollisionProxy::IntersectSegment` | 34.45% | Work in progress |
| `0x004d8300` | `CollisionProxy::SetPrimitivePosition` | 100.00% | Work in progress |
| `0x004d8800` | `DisplayRenderer::AddSupportedTextureFormat` | 50.00% | Work in progress |
| `0x004d8840` | `DisplayRenderer::InitializeTextureFormats` | 100.00% | Verified exact |
| `0x004d8950` | `ResampleBitmapImage` | 0.67% | Work in progress |
| `0x004d8c90` | `DisplayRenderer::CopyBitmapMipToLockedSurface` | 4.29% | Work in progress |
| `0x004d9030` | `DisplayRenderer::GetTextureMipLevelCount` | 100.00% | Verified exact |
| `0x004d9050` | `DisplayRenderer::CreatePrimaryTexture` | 7.10% | Work in progress |
| `0x004d9160` | `DisplayRenderer::CreateSecondaryTexture` | 35.85% | Work in progress |
| `0x004d92a0` | `UploadBitmapMipChain` | 20.52% | Work in progress |
| `0x004d9610` | `RendererTextureRecord::DropTopMip` | 10.99% | Work in progress |
| `0x004d9970` | `DisplayRenderer::SetTextureRecordDimensions` | 31.13% | Work in progress |
| `0x004d9ae0` | `DisplayRenderer::AdjustTextureDimensions` | 60.61% | Work in progress |
| `0x004d9b70` | `DisplayRenderer::PrepareBitmapForTexture` | 1.68% | Work in progress |
| `0x004d9cb0` | `DisplayRenderer::LoadTextureImage` | 77.14% | Work in progress |
| `0x004d9dc0` | `DisplayRenderer::LoadCubeTextureFace` | 61.34% | Work in progress |
| `0x004d9f20` | `DisplayRenderer::CreateTextureRecord` | 79.01% | Work in progress |
| `0x004da020` | `RendererTextureRecord::UpdateGpuTexture` | 14.60% | Work in progress |
| `0x004da1e0` | `DisplayRenderer::ReleaseTextureRecord` | 56.25% | Work in progress |
| `0x004da240` | `DisplayRenderer::SelectTextureFormat` | 40.00% | Work in progress |
| `0x004da490` | `RendererTextureRecord::LockPrimarySurface` | 57.89% | Work in progress |
| `0x004da500` | `RendererTextureRecord::UnlockPrimarySurface` | 80.85% | Work in progress |
| `0x004da540` | `RendererTextureRecord::UploadBitmapImage` | 34.86% | Work in progress |
| `0x004da5e0` | `DisplayRenderer::BuildMipSidecar` | 34.70% | Work in progress |
| `0x004da9f0` | `?ReadBitmapInfoSettings@@YAIPBDPAUBitmapInfoSettings@?%Z:\home\ilya\work\lr2_decomp\racers2\LEGO Racers 2\include\bitmapimage.h295983220@@@Z` | 2.29% | Work in progress |
| `0x004dabe0` | `ParseBitmapInfoValue` | 40.31% | Work in progress |
| `0x004dac90` | `GenerateBitmapMipLevel` | 17.20% | Work in progress |
| `0x004db5c0` | `FindClosestPaletteColor` | 30.09% | Work in progress |
| `0x004db650` | `GenerateNormalMap` | 12.59% | Work in progress |
| `0x004db900` | `DisplayRenderer::SetTextureIndexMap` | 100.00% | Verified exact |
| `0x004db920` | `UpdateTextureRecordFormat` | 8.00% | Work in progress |
| `0x004db9c0` | `DisplayRenderer::ReleaseDeviceResources` | 91.25% | Work in progress |
| `0x004dbab0` | `DisplayRenderer::SetDeviceResourceMode` | 66.29% | Work in progress |
| `0x004dbbe0` | `DisplayRenderer::RefreshTextureFormats` | 13.39% | Work in progress |
| `0x004dbfb0` | `RendererTextureRecord::ReleaseTexture` | 66.67% | Work in progress |
| `0x004dc000` | `DisplayRenderer::RestoreDeviceResources` | 37.88% | Work in progress |
| `0x004dc250` | `DisplayRenderer::AdvanceDeviceFallback` | 41.45% | Work in progress |
| `0x004dc4c0` | `DisplayRenderer::ShouldUseHighResolutionTextures` | 98.11% | Reviewed near match |
| `0x004dc510` | `DisplayRenderer::SetTextureQuality` | 97.14% | Reviewed near match |
| `0x004dc590` | `DisplayRenderer::SetTextureFiltering` | 45.95% | Work in progress |
| `0x004dc6e0` | `DisplayRenderer::SetFogEnabled` | 24.24% | Work in progress |
| `0x004dc740` | `DisplayRenderer::SetFogColor` | 11.43% | Work in progress |
| `0x004dc790` | `DisplayRenderer::SetFogRange` | 37.21% | Work in progress |
| `0x004dc7f0` | `DisplayRenderer::SetDepthState` | 26.67% | Work in progress |
| `0x004dc8c0` | `DisplayRenderer::SetAlphaTestReference` | 54.55% | Work in progress |
| `0x004dc900` | `DisplayRenderer::SetDepthAlwaysWrite` | 33.33% | Work in progress |
| `0x004dc960` | `DisplayRenderer::AdjustSolidPassScope` | 100.00% | Verified exact |
| `0x004dc990` | `DisplayRenderer::IsSolidPassOverrideActive` | 31.58% | Work in progress |
| `0x004dc9b0` | `DisplayRenderer::ClearBuffers` | 35.91% | Work in progress |
| `0x004dccc0` | `DisplayRenderer::SetAlphaBlendMode` | 8.94% | Work in progress |
| `0x004dcf30` | `DisplayRenderer::PrepareDrawPacketState` | 47.41% | Work in progress |
| `0x004dd0b0` | `DisplayRenderer::ApplyAuxiliaryMaterial` | 91.57% | Work in progress |
| `0x004dd130` | `DisplayRenderer::BeginScene` | 68.29% | Work in progress |
| `0x004dd230` | `DisplayRenderer::EndScene` | 100.00% | Verified exact |
| `0x004dd240` | `RendererTimingStatisticsState::GetMetrics` | 20.42% | Work in progress |
| `0x004dd460` | `DisplayRenderer::DrawRendererPacket` | 40.99% | Work in progress |
| `0x004dd900` | `DisplayRenderer::DrawIndexedPacket` | 31.08% | Work in progress |
| `0x004dd9f0` | `DisplayRenderer::IsPointWithinViewport` | 38.76% | Work in progress |
| `0x004ddad0` | `MultiplyMatrix4` | 100.00% | Verified exact |
| `0x004ddb30` | `DisplayRenderer::ScaleProjectionCoordinate` | 100.00% | Verified exact |
| `0x004ddb70` | `TransformStaticWorldViewPoint` | 15.58% | Work in progress |
| `0x004ddc00` | `DisplayRenderer::TransformProjectedPoint` | 2.82% | Work in progress |
| `0x004ddce0` | `DisplayRenderer::TransformViewportPoint` | 2.82% | Work in progress |
| `0x004ddeb0` | `MultiplyTextureMatrixInPlace` | 48.35% | Work in progress |
| `0x004ddf30` | `DisplayRenderer::SetWorldTransform(class Quaternion const *, class Vector3 const *)` | 36.46% | Work in progress |
| `0x004de080` | `DisplayRenderer::SetWorldTransform(class Matrix3 const *, class Vector3 const *)` | 13.33% | Work in progress |
| `0x004de110` | `ApplyDefaultWorldTransform` | 70.83% | Work in progress |
| `0x004de160` | `DisplayRenderer::SetViewTransform` | 13.58% | Work in progress |
| `0x004de2e0` | `DisplayRenderer::SetProjection` | 41.45% | Work in progress |
| `0x004de460` | `DisplayRenderer::BuildBumpMapVectors` | 9.51% | Work in progress |
| `0x004de990` | `ApplySpecialDrawPacketEffect` | 20.03% | Work in progress |
| `0x004dee20` | `DisplayRenderer::SetStencilBufferEnabled` | 40.00% | Work in progress |
| `0x004dee90` | `DisplayRenderer::ResetStencilOperations` | 81.63% | Work in progress |
| `0x004deed0` | `DisplayRenderer::SetStencilFunction` | 13.51% | Work in progress |
| `0x004deff0` | `DisplayRenderer::SetStencilOperation` | 17.33% | Work in progress |
| `0x004df190` | `DisplayRenderer::SetColorOverride` | 15.38% | Work in progress |
| `0x004df220` | `DisplayRenderer::DisableColorOverride` | 100.00% | Verified exact |
| `0x004df230` | `DisplayRenderer::SetBlendModeOverride` | 100.00% | Verified exact |
| `0x004df250` | `DisplayRenderer::ClearBlendModeOverride` | 100.00% | Verified exact |
| `0x004df260` | `DisplayRenderer::DrawGuiRectangle` | 25.51% | Work in progress |
| `0x004df6b0` | `DisplayRenderer::BeginSpriteBatch` | 93.33% | Work in progress |
| `0x004df6e0` | `DisplayRenderer::DrawSprite` | 75.00% | Work in progress |
| `0x004df720` | `DisplayRenderer::AllocateSpriteVertices` | 62.50% | Work in progress |
| `0x004df760` | `FlushSpecularVertexStream` | 100.00% | Verified exact |
| `0x004df7d0` | `DisplayRenderer::DrawSpriteGeometry` | 14.58% | Work in progress |
| `0x004dfb20` | `DisplayRenderer::Present` | 100.00% | Verified exact |
| `0x004dfb60` | `DisplayRenderer::ApplyTextureTransform` | 24.90% | Work in progress |
| `0x004dfd10` | `DisplayRenderer::UpdateTextureStageFlags` | 100.00% | Verified exact |
| `0x004dfd80` | `DisplayRenderer::SetTextureRenderTarget` | 85.87% | Work in progress |
| `0x004dfec0` | `DisplayRenderer::SetAmbientColorFloats` | 68.42% | Work in progress |
| `0x004dff90` | `DisplayRenderer::GetStageFactorColor` | 100.00% | Verified exact |
| `0x004dffa0` | `DisplayRenderer::GetInverseStageFactorColor` | 100.00% | Verified exact |
| `0x004dffb0` | `DisplayRenderer::DrawStencilVolumes` | 50.00% | Work in progress |
| `0x004e00d0` | `DisplayRenderer::DrawStencilOverlay` | 33.78% | Work in progress |
| `0x004e02d0` | `DisplayRenderer::DrawUnlitLines` | 39.67% | Work in progress |
| `0x004e03d0` | `DisplayRenderer::DrawUnlitTriangles` | 20.22% | Work in progress |
| `0x004e0560` | `DisplayRenderer::ApplyTextureOperation` | 31.77% | Work in progress |
| `0x004e0d40` | `DisplayRenderer::SetVertexStream` | 56.72% | Work in progress |
| `0x004e0db0` | `DisplayRenderer::DrawQuadPacket` | 9.31% | Work in progress |
| `0x004e18a0` | `DisplayRenderer::SetTextureTransform` | 6.59% | Work in progress |
| `0x004e1950` | `PassThroughStageValidationResult` | 100.00% | Verified exact |
| `0x004e1960` | `RendererTextureStageManager::Initialize` | 92.36% | Work in progress |
| `0x004e1c30` | `RendererTextureStageManager::MaskWithoutHighestPriorityStage` | 96.97% | Reviewed near match |
| `0x004e1dd0` | `RendererTextureStageManager::AreTextureOperationsSupported` | 37.74% | Work in progress |
| `0x004e1e20` | `RendererTextureStageManager::ApplyStagePlanSubrecord` | 34.62% | Work in progress |
| `0x004e1e60` | `DisplayRenderer::BindMappedTexture` | 46.81% | Work in progress |
| `0x004e1f20` | `DisplayRenderer::ShouldUseTextureResourceIndex` | 20.69% | Work in progress |
| `0x004e1f50` | `DisplayRenderer::IsTextureResourceKindAccepted` | 32.26% | Work in progress |
| `0x004e1f80` | `DisplayRenderer::IsTextureResourceKindSpecial` | 26.67% | Work in progress |
| `0x004e1fa0` | `DisplayRenderer::RestoreTextureStageDefaults` | 34.00% | Work in progress |
| `0x004e2490` | `ResetTextureStageCoordinateIndices` | 92.86% | Work in progress |
| `0x004e24c0` | `RendererTextureStageManager::FindSubrecordGroup` | 75.00% | Work in progress |
| `0x004e24f0` | `ResolveRendererTextureStagePlan` | 39.53% | Work in progress |
| `0x004e2730` | `CopyTextureStageDefaultSlot` | 100.00% | Verified exact |
| `0x004e2750` | `RemoveStageFromOperationPlan` | 25.88% | Work in progress |
| `0x004e27c0` | `RendererTextureStageManager::CreateSubrecordGroup` | 25.64% | Work in progress |
| `0x004e2850` | `RendererTextureStageManager::BuildDefaultStagePlan` | 9.05% | Work in progress |
| `0x004e4e80` | `InitializeTextureStageRecord` | 100.00% | Verified exact |
| `0x004e4ea0` | `SetTextureStageProfileValue` | 100.00% | Verified exact |
| `0x004e4fd0` | `RendererTextureStageManager::BuildProfileStagePlan` | 9.56% | Work in progress |
| `0x004e5e70` | `InitializeAndLinkTextureStageRecord` | 15.38% | Work in progress |
| `0x004e5ea0` | `LinkTextureStageRecord` | 100.00% | Verified exact |
| `0x004e5eb0` | `AddTextureStageSubrecords` | 42.86% | Work in progress |
| `0x004e5f30` | `ApplyTextureStagePlanForManager` | 94.55% | Work in progress |
| `0x004e5f90` | `ApplyRendererTextureStagePlan` | 42.29% | Work in progress |
| `0x004e6360` | `RendererTextureStageManager::CopySubrecordGroupCacheEntry` | 100.00% | Verified exact |
| `0x004e6380` | `DisplayRenderer::SupportsDotProduct3` | 22.22% | Work in progress |
| `0x004e66c0` | `ContactCacheController::ContactCacheController` | 17.39% | Work in progress |
| `0x004e6730` | `ContactCacheAllocationSize` | 100.00% | Work in progress |
| `0x004e6740` | `DeleteContactCacheController` | 20.00% | Work in progress |
| `0x004e67c0` | `GameManager::MoveObjects` | 26.09% | Work in progress |
| `0x004e6a00` | `GameManager::PrepareComponents` | 71.79% | Work in progress |
| `0x004e6a30` | `GameObject::RefreshCollisionState` | 45.65% | Work in progress |
| `0x004e6aa0` | `GameManager::RefreshCollisionStates` | 100.00% | Work in progress |
| `0x004e6ad0` | `GameManager::SolveContacts` | 15.76% | Work in progress |
| `0x004e71c0` | `BeginProfileRange` | 5.38% | Work in progress |
| `0x004e77f0` | `EndProfileRange` | 6.09% | Work in progress |
| `0x004e7ae0` | `GameManager::SetControllerFrame` | 72.00% | Work in progress |
| `0x004e7b10` | `InitializeRendererFloatingPoint` | 71.43% | Work in progress |
| `0x004e7b30` | `SaveFloatingPointControl` | 100.00% | Verified exact |
| `0x004e7b40` | `RestoreFloatingPointControl` | 100.00% | Verified exact |
| `0x004e7b50` | `ShowDiagnosticDialog` | 100.00% | Verified exact |
| `0x004e7b90` | `AllocateAlignedMemory` | 100.00% | Verified exact |
| `0x004e7bb0` | `DecompressGtcBlock` | 9.78% | Work in progress |
| `0x004e8830` | `FinishFrameAudioState` | 62.96% | Work in progress |
| `0x004e9390` | `ForwardPositionToChildGeometry` | 100.00% | Work in progress |
| `0x004e9420` | `DisplayRenderer::TryReuseVertexStorage` | 67.89% | Work in progress |
| `0x004e94b0` | `DisplayRenderer::CreateVertexBuffer` | 42.28% | Work in progress |
| `0x004e9540` | `RendererVertexBuffer::Release` | 100.00% | Verified exact |
| `0x004e9640` | `DisplayRenderer::AllocateVertexStorage` | 55.38% | Work in progress |
| `0x004e96a0` | `RendererVertexBuffer::ConfigureFromStorage` | 26.97% | Work in progress |
| `0x004e9770` | `RendererVertexBuffer::Lock` | 81.20% | Work in progress |
| `0x004e9810` | `RendererVertexBuffer::Unlock` | 42.28% | Work in progress |
| `0x004e9960` | `RendererVertexBuffer::CopyFromVertexData` | 31.07% | Work in progress |
| `0x004e9b20` | `RendererVertexBuffer::CopyVerticesFrom` | 14.39% | Work in progress |
| `0x004e9d00` | `RendererVertexBuffer::CopyToVertexData` | 40.00% | Work in progress |
| `0x004e9d80` | `RendererVertexBuffer::SetBufferPolicy` | 13.64% | Work in progress |
| `0x004e9db0` | `DisplayRenderer::PrepareDrawPlan` | 80.85% | Work in progress |
| `0x004e9df0` | `RendererVertexBuffer::SetMode` | 100.00% | Verified exact |
| `0x004e9e10` | `DisplayRenderer::CreateIndexBuffer` | 30.86% | Work in progress |
| `0x004e9f20` | `RendererIndexBuffer::ReleaseDeviceBuffer` | 100.00% | Verified exact |
| `0x004e9f40` | `RendererIndexBuffer::Upload` | 45.78% | Work in progress |
| `0x004e9fa0` | `RendererIndexBuffer::SetBufferPolicy` | 4.65% | Work in progress |
| `0x004e9fd0` | `GetVertexFormat` | 100.00% | Verified exact |
| `0x004ea000` | `GetVertexFormatStride` | 100.00% | Verified exact |
| `0x004ea080` | `RendererVertexStorage::Initialize` | 50.47% | Work in progress |
| `0x004ea1e0` | `RendererVertexStorage::Reserve` | 92.68% | Work in progress |
| `0x004ea220` | `RendererVertexStorage::Lock` | 94.94% | Work in progress |
| `0x004ea2d0` | `RendererVertexStorage::Unlock` | 100.00% | Verified exact |
| `0x004ea2f0` | `RendererVertexStorage::ReleaseReference` | 73.68% | Work in progress |
| `0x004ea340` | `RendererVertexStorage::ReleaseDeviceBuffer` | 100.00% | Verified exact |
| `0x004ea360` | `RendererIndexStorage::Initialize` | 72.34% | Work in progress |
| `0x004ea3b0` | `RendererIndexStorage::Reserve` | 100.00% | Verified exact |
| `0x004ea3e0` | `RendererIndexStorage::ReleaseReference` | 69.39% | Work in progress |
| `0x004ea440` | `RendererAnimationResourceRecord::ReleaseResourceReference` | 100.00% | Verified exact |
| `0x004ea460` | `DisplayRenderer::FindOrAllocateAnimationResource` | 42.67% | Work in progress |
| `0x004ea550` | `UpdateTerrainGridEdgeSelections` | 17.68% | Work in progress |
| `0x004ea690` | `DisplayRenderer::DrawTerrain` | 10.21% | Work in progress |
| `0x004eb530` | `DisplayRenderer::BuildTerrainGridStream` | 6.25% | Work in progress |
| `0x004eb9d0` | `BuildTerrainGeometry` | 7.03% | Work in progress |
| `0x004ec2c0` | `DisplayRenderer::ConfigureStreamResources` | 30.00% | Work in progress |
| `0x004ec3f0` | `MouseInputDevice::SetRelativeAxisRange` | 100.00% | Verified exact |
| `0x004ec450` | `EnumerateMouseAxisObject` | 100.00% | Verified exact |
| `0x004ec470` | `MouseInputDevice::ReadState` | 100.00% | Verified exact |
| `0x004ec4d0` | `MouseInputDevice::IsButtonPressed` | 100.00% | Verified exact |
| `0x004ec500` | `MouseInputDevice::GetAxisValue` | 47.76% | Work in progress |
| `0x004ec590` | `KeyboardInputDevice::ReadState` | 72.92% | Work in progress |
| `0x004ec630` | `KeyboardInputDevice::IsKeyPressed` | 100.00% | Verified exact |
| `0x004ec670` | `JoystickEffect::JoystickEffect(void)` | 100.00% | Verified exact |
| `0x004ec680` | `JoystickEffect::JoystickEffect(struct IDirectInputDevice8A *, unsigned int, unsigned int)` | 91.09% | Work in progress |
| `0x004ec820` | `BuildConstantForceDescription` | 3.30% | Work in progress |
| `0x004eca50` | `BuildSpringDescription` | 5.71% | Work in progress |
| `0x004ecbe0` | `BuildFrictionDescription` | 5.71% | Work in progress |
| `0x004ecd70` | `BuildEnvelopedConstantDescription` | 2.63% | Work in progress |
| `0x004ed030` | `BuildSineDescription` | 9.94% | Work in progress |
| `0x004ed250` | `BuildEnvelopedSineDescription` | 10.40% | Work in progress |
| `0x004ed520` | `JoystickEffect::SetConstantForce` | 61.76% | Work in progress |
| `0x004ed580` | `JoystickEffect::SetSpringCoefficient` | 67.65% | Work in progress |
| `0x004ed5e0` | `JoystickEffect::SetFrictionCoefficient` | 67.65% | Work in progress |
| `0x004ed640` | `JoystickEffect::SetEnvelopeConstantForce` | 58.82% | Work in progress |
| `0x004ed6a0` | `JoystickEffect::SetPeriodicParameters` | 52.78% | Work in progress |
| `0x004ed700` | `JoystickEffect::SetEnvelopePeriodicParameters` | 77.78% | Work in progress |
| `0x004ed760` | `JoystickEffect::SetSquareParameters` | 75.00% | Work in progress |
| `0x004ed7c0` | `JoystickEffect::Stop` | 78.43% | Work in progress |
| `0x004ed810` | `JoystickEffect::Release` | 100.00% | Verified exact |
| `0x004ed850` | `JoystickInputDevice::Shutdown` | 100.00% | Verified exact |
| `0x004ed8c0` | `JoystickInputDevice::JoystickInputDevice` | 60.00% | Work in progress |
| `0x004ed940` | `JoystickInputDevice::HasForceFeedbackAxes` | 100.00% | Verified exact |
| `0x004ed950` | `JoystickInputDevice::BindAxes` | 100.00% | Verified exact |
| `0x004ed9a0` | `JoystickInputDevice::Initialize` | 67.47% | Work in progress |
| `0x004edb50` | `EnumerateJoystickDevice` | 40.00% | Work in progress |
| `0x004edbd0` | `EnumerateJoystickAxisObject` | 100.00% | Verified exact |
| `0x004edbf0` | `JoystickAxis::JoystickAxis` | 100.00% | Verified exact |
| `0x004edc10` | `JoystickAxis::Bind` | 100.00% | Verified exact |
| `0x004edc50` | `JoystickAxis::SetRange` | 53.70% | Work in progress |
| `0x004edcf0` | `JoystickAxis::GetNormalizedValue` | 100.00% | Verified exact |
| `0x004edd20` | `JoystickInputDevice::CountForceFeedbackAxis` | 100.00% | Verified exact |
| `0x004edd50` | `JoystickInputDevice::ReadState` | 54.72% | Work in progress |
| `0x004edec0` | `JoystickInputDevice::StopAllEffects` | 100.00% | Verified exact |
| `0x004edee0` | `JoystickInputDevice::EnableForceFeedback` | 84.21% | Work in progress |
| `0x004edf10` | `JoystickInputDevice::GetAxisCount` | 100.00% | Verified exact |
| `0x004edf20` | `JoystickInputDevice::UpdateAutoRange` | 42.42% | Work in progress |
| `0x004edfc0` | `JoystickAxis::ExpandRange` | 100.00% | Verified exact |
| `0x004ee020` | `JoystickInputDevice::IsButtonPressed` | 100.00% | Verified exact |
| `0x004ee050` | `InputDeviceRecord::GetButtonValue` | 95.24% | Reviewed near match |
| `0x004ee080` | `JoystickInputDevice::GetAxisValue` | 100.00% | Verified exact |
| `0x004ee0c0` | `JoystickInputDevice::Acquire` | 100.00% | Verified exact |
| `0x004ee110` | `JoystickInputDevice::Unacquire` | 100.00% | Verified exact |
| `0x004ee160` | `JoystickInputDevice::ResetForceFeedback` | 100.00% | Verified exact |
| `0x004ee190` | `JoystickInputDevice::InitializeEffects` | 72.44% | Work in progress |
| `0x004ee3d0` | `JoystickInputDevice::SetConstantForce` | 32.00% | Work in progress |
| `0x004ee430` | `JoystickInputDevice::SetSpring` | 100.00% | Verified exact |
| `0x004ee450` | `JoystickInputDevice::SetFriction` | 100.00% | Verified exact |
| `0x004ee470` | `JoystickInputDevice::SetSine` | 100.00% | Verified exact |
| `0x004ee4a0` | `JoystickInputDevice::SetEnvelopedConstantForce` | 100.00% | Verified exact |
| `0x004ee4c0` | `JoystickInputDevice::SetSquare` | 100.00% | Verified exact |
| `0x004ee4f0` | `JoystickInputDevice::SetEnvelopedSine` | 100.00% | Verified exact |
| `0x004ee520` | `JoystickInputDevice::StopConstantForce` | 100.00% | Verified exact |
| `0x004ee540` | `JoystickInputDevice::StopEnvelopedSine` | 100.00% | Verified exact |
| `0x004ee560` | `JoystickInputDevice::StopSpring` | 100.00% | Verified exact |
| `0x004ee580` | `JoystickInputDevice::StopFriction` | 100.00% | Verified exact |
| `0x004ee5a0` | `JoystickInputDevice::StopSine` | 100.00% | Verified exact |
| `0x004ee5c0` | `JoystickInputDevice::StopSquare` | 100.00% | Verified exact |
| `0x004ee5e0` | `JoystickInputDevice::StopEnvelopedConstantForce` | 100.00% | Verified exact |
| `0x004ee600` | `JoystickInputDevice::ReleaseEffects` | 100.00% | Verified exact |
| `0x004ee660` | `JoystickInputDevice::SetAutoCenter` | 100.00% | Verified exact |
| `0x004ee700` | `InitializeAudioCommentary` | 39.22% | Work in progress |
| `0x004ee7a0` | `ShutdownAudioCommentary` | 51.28% | Work in progress |
| `0x004ee7e0` | `SetAudioCommentaryGain` | 21.05% | Work in progress |
| `0x004ee800` | `QueueAudioCommentary` | 23.08% | Work in progress |
| `0x004ee820` | `StopAudioCommentary` | 35.71% | Work in progress |
| `0x004ee880` | `EnqueueAudioCommentary` | 43.24% | Work in progress |
| `0x004ee900` | `PlayNextAudioCommentary` | 12.05% | Work in progress |
| `0x004ee9b0` | `InitializeAudioPcmBuffer` | 72.62% | Work in progress |
| `0x004eeaa0` | `ShutdownAudioPcmBuffer` | 60.61% | Work in progress |
| `0x004eead0` | `AppendAudioPcmData` | 46.63% | Work in progress |
| `0x004eec50` | `AudioService::PlayCdTrack` | 28.05% | Work in progress |
| `0x004eedf0` | `AudioService::SetMixerVolume` | 87.76% | Work in progress |
| `0x004ef060` | `AudioService::GetMixerVolume` | 66.45% | Work in progress |
| `0x004ef2a0` | `AudioService::SetCdPlayerMode` | 64.81% | Work in progress |
| `0x004ef360` | `AudioService::SetDigitalAudioPath` | 96.30% | Work in progress |
| `0x004ef560` | `SetDigitalMusicGain` | 87.18% | Work in progress |
| `0x004ef5a0` | `GetDigitalMusicGain` | 100.00% | Verified exact |
| `0x004ef630` | `CheckAudioMediaResult` | 15.74% | Work in progress |
| `0x004ef930` | `ThreeDimensionalDirectSoundProvider::ThreeDimensionalDirectSoundProvider` | 72.22% | Work in progress |
| `0x004ef9b0` | `DeleteBaseAudioProvider` | 91.67% | Work in progress |
| `0x004ef9d0` | `ThreeDimensionalDirectSoundProvider::Initialize` | 89.89% | Work in progress |
| `0x004efab0` | `ThreeDimensionalDirectSoundProvider::ReleaseDeviceInterfaces` | 100.00% | Work in progress |
| `0x004efb20` | `CreateThreeDimensionalSourceBuffer` | 0.00% | Work in progress |
| `0x004efd60` | `ReleaseThreeDimensionalSourceBuffer` | 80.00% | Work in progress |
| `0x004efdd0` | `GetWaveSampleRate` | 96.88% | Work in progress |
| `0x004efe40` | `StopAudioPlaybackObject` | 100.00% | Work in progress |
| `0x004efee0` | `UpdateThreeDimensionalAudioSource` | 29.49% | Work in progress |
| `0x004f02f0` | `EnsureThreeDimensionalBuffer` | 50.00% | Work in progress |
| `0x004f04e0` | `StereoDirectSoundProvider::StereoDirectSoundProvider` | 76.92% | Work in progress |
| `0x004f0550` | `StereoDirectSoundProvider::Initialize` | 0.00% | Work in progress |
| `0x004f0690` | `ReleaseDirectSoundOutputInterfaces` | 100.00% | Work in progress |
| `0x004f06c0` | `ReleaseStereoSourceBuffer` | 86.67% | Work in progress |
| `0x004f06f0` | `CanStopStereoAudioBuffer` | 93.88% | Work in progress |
| `0x004f0730` | `UpdateStereoAudioSource` | 25.87% | Work in progress |
| `0x004f0aa0` | `MonoDirectSoundProvider::MonoDirectSoundProvider` | 76.92% | Work in progress |
| `0x004f0b10` | `MonoDirectSoundProvider::Initialize` | 0.00% | Work in progress |
| `0x004f0c50` | `CreatePcmSourceBuffer` | 0.00% | Work in progress |
| `0x004f0e30` | `MonoDirectSoundProvider::ReleasePlaybackObject` | 100.00% | Work in progress |
| `0x004f0ea0` | `MonoDirectSoundProvider::ReleaseSourcePlaybackBuffer` | 70.59% | Work in progress |
| `0x004f0ed0` | `StartAudioPlaybackObject` | 100.00% | Work in progress |
| `0x004f0f20` | `IsAudioPlaybackObjectActive` | 100.00% | Work in progress |
| `0x004f0f50` | `SetAudioPlaybackFrequency` | 69.14% | Work in progress |
| `0x004f0fb0` | `ResetAudioPlaybackPosition` | 100.00% | Work in progress |
| `0x004f0fd0` | `UpdateMonoAudioSource` | 44.92% | Work in progress |
| `0x004f11a0` | `FindAudioGameObject` | 100.00% | Verified exact |
| `0x004f11c0` | `HasAudioGameObjectPosition` | 100.00% | Verified exact |
| `0x004f11e0` | `GetAudioGameObjectPosition` | 44.00% | Work in progress |
| `0x004f1240` | `CopyAudioSpatialWorldPosition` | 100.00% | Work in progress |
| `0x004f1270` | `CopyAudioSpatialBasis` | 17.86% | Work in progress |
| `0x004f12e0` | `GetAudioFrameDuration` | 22.22% | Work in progress |
| `0x004f1860` | `DigitalMusicController::StartPlayback` | 92.59% | Work in progress |
| `0x004f18c0` | `DigitalMusicController::StopPlaybackIfActive` | 82.76% | Work in progress |
| `0x004f1900` | `DigitalMusicController::SetEnabled` | 76.36% | Work in progress |
| `0x004f1970` | `DigitalMusicController::ResetFileAndStreamState` | 88.00% | Work in progress |
| `0x004f19b0` | `DigitalMusicController::StopAndReset` | 100.00% | Work in progress |
| `0x004f19d0` | `DigitalMusicController::UpdatePlaybackBuffer` | 60.23% | Work in progress |
| `0x004f1bc0` | `DigitalMusicController::StopAndReleasePlaybackBuffer` | 100.00% | Work in progress |
| `0x004f1c00` | `RestoreDigitalMusicOutput` | 32.95% | Work in progress |
| `0x004f1d40` | `SetDigitalMusicBufferGain` | 88.24% | Work in progress |
| `0x004f1d70` | `DigitalMusicController::DecodeAdpcmBlock` | 10.78% | Work in progress |
| `0x004f1fa0` | `DigitalMusicController::CopyDecodedSamples` | 30.86% | Work in progress |
| `0x004f2070` | `DigitalMusicController::ReadFileBytes` | 67.53% | Work in progress |
| `0x004f20c0` | `DigitalMusicController::SkipFileBytes` | 100.00% | Work in progress |
| `0x004f2110` | `CommentaryAudioStream::CommentaryAudioStream` | 17.39% | Work in progress |
| `0x004f2250` | `CommentaryAudioStream::~CommentaryAudioStream` | 100.00% | Verified exact |
| `0x004f2270` | `CommentaryAudioStream::UpdateBuffer` | 39.82% | Work in progress |
| `0x004f2450` | `CommentaryAudioStream::Play` | 23.88% | Work in progress |
| `0x004f24e0` | `CommentaryAudioStream::Stop` | 67.39% | Work in progress |
| `0x004f2560` | `CommentaryAudioStream::SetGain` | 86.67% | Work in progress |
| `0x004f2590` | `CommentaryAudioStream::IsReading` | 100.00% | Verified exact |
| `0x004f25a0` | `BeginAudioRead` | 78.16% | Work in progress |
| `0x004f2610` | `WaitForAudioRead` | 100.00% | Verified exact |
| `0x004f2690` | `CountPendingAudioReads` | 88.89% | Work in progress |
| `0x004f26b0` | `FindCompletedAudioRead` | 97.96% | Reviewed near match |
| `0x004f2730` | `GetAudioReadRequest` | 100.00% | Verified exact |
| `0x004f2740` | `CommentaryAudioStream::StartRead` | 44.68% | Work in progress |
| `0x004f2990` | `CommentaryAudioStream::FinishRead` | 75.00% | Work in progress |
| `0x004f29f0` | `CommentaryAudioStream::DecodePcm` | 17.96% | Work in progress |
| `0x004f2ea0` | `CommentaryAudioStream::ReleasePlaybackBuffer` | 100.00% | Verified exact |
| `0x004f2ec0` | `RestoreCommentaryPlaybackBuffer` | 30.08% | Work in progress |
| `0x004f4a40` | `InitializeMultimediaTimerService` | 53.01% | Work in progress |
| `0x004f4ad0` | `ShutdownMultimediaTimerService` | 100.00% | Verified exact |
| `0x004f4af0` | `MultimediaTimerService::EndPeriod` | 100.00% | Verified exact |
| `0x004f4b10` | `MultimediaTimerService::CreatePeriodicEvent` | 100.00% | Verified exact |
| `0x004f4b40` | `MultimediaTimerService::KillPeriodicEvent` | 100.00% | Verified exact |
| `0x004f4b60` | `MultimediaTimerService::CreateNamedMutex` | 100.00% | Verified exact |
| `0x004f4b90` | `MultimediaTimerService::CloseMutex` | 100.00% | Verified exact |
| `0x004f4bb0` | `MultimediaTimerService::AcquireNamedMutex` | 67.80% | Work in progress |
| `0x004f4c00` | `MultimediaTimerService::ReleaseNamedMutex` | 100.00% | Verified exact |
| `0x004f4c50` | `CollisionServiceDetachAll` | 100.00% | Work in progress |
| `0x004f4c80` | `CollisionServiceSetPrimitivePosition` | 100.00% | Work in progress |
| `0x004f4ca0` | `CollisionServiceSetTransform` | 100.00% | Work in progress |
| `0x004f4cf0` | `CollisionProxyFromService` | 24.49% | Work in progress |
| `0x004f4dc0` | `DestroyCollisionProxyFromService` | 100.00% | Work in progress |
| `0x004f4e00` | `AttachCollisionProxyFromService` | 100.00% | Work in progress |
| `0x004f4e40` | `CollisionServicePrepareQuery` | 84.21% | Work in progress |
| `0x004f4e70` | `CollisionServiceQuerySphere` | 85.71% | Work in progress |
| `0x004f4e90` | `CollisionServiceQuerySegment` | 85.71% | Work in progress |
| `0x004f4eb0` | `CollisionServiceSweep` | 22.22% | Work in progress |
| `0x004f50e0` | `CollisionModelTransform::ResetIdentity` | 31.58% | Work in progress |
| `0x004f5140` | `SpatialLinkRegistry::SpatialLinkRegistry` | 90.00% | Work in progress |
| `0x004f5160` | `SpatialLinkRegistry::Insert` | 80.00% | Work in progress |
| `0x004f5180` | `SpatialLinkRegistry::Remove` | 100.00% | Verified exact |
| `0x004f51b0` | `CollisionSpatialIndex::CollisionSpatialIndex` | 63.95% | Work in progress |
| `0x004f5260` | `CollisionSpatialIndex::InsertStatic` | 100.00% | Work in progress |
| `0x004f5290` | `CollisionSpatialIndex::InsertDynamic` | 100.00% | Work in progress |
| `0x004f52d0` | `CollisionSpatialIndex::RemoveStatic` | 100.00% | Work in progress |
| `0x004f52f0` | `CollisionSpatialIndex::RemoveDynamic` | 100.00% | Work in progress |
| `0x004f5330` | `CollisionSpatialIndex::UpdateStatic` | 100.00% | Work in progress |
| `0x004f5340` | `CollisionSpatialIndex::UpdateDynamic` | 72.22% | Work in progress |
| `0x004f5370` | `PrepareCollisionSpatialQueries` | 100.00% | Work in progress |
| `0x004f53a0` | `QueryCollisionSphere` | 88.89% | Work in progress |
| `0x004f53e0` | `QueryCollisionSegment` | 93.91% | Work in progress |
| `0x004f54a0` | `BitmapImage::LoadTga` | 17.27% | Work in progress |
| `0x004f59c0` | `BitmapImage::ReleaseStorage` | 95.24% | Reviewed near match |
| `0x004f5a10` | `WriteBitmapTgaFile` | 30.77% | Work in progress |
| `0x004f6310` | `InitializePlatformServices` | 70.83% | Work in progress |
| `0x004f6360` | `ShutdownPlatformServices` | 88.89% | Work in progress |
| `0x004f8470` | `IntersectTerrainTriangle` | 18.18% | Work in progress |
| `0x004f88f0` | `TraverseTerrainGridRay` | 24.22% | Work in progress |
| `0x004f8ab0` | `ClipTerrainGridSegment` | 29.57% | Work in progress |
| `0x004f8c40` | `ClipTerrainGridBoundary` | 29.46% | Work in progress |
| `0x004f8da0` | `TraverseCollisionGridRay` | 78.79% | Work in progress |
| `0x004f90e0` | `GetSpatialGridShift` | 90.91% | Work in progress |
| `0x004f96a0` | `TestCollisionGeometryPair` | 20.00% | Work in progress |
| `0x004f97f0` | `TestCollisionGeometryAgainstSphere` | 45.16% | Work in progress |
| `0x004f9d50` | `TestCollisionGeometryAgainstBox` | 42.55% | Work in progress |
| `0x004fa210` | `IntersectCollisionGeometrySegment` | 63.08% | Work in progress |
| `0x004fa2b0` | `TestGeometryBoxContact` | 10.75% | Work in progress |
| `0x004fa7f0` | `CollisionChildDefinitions::CreateProxies` | 68.66% | Work in progress |
| `0x004faf60` | `IntersectTerrainCollisionSegment` | 29.06% | Work in progress |
| `0x004fb3e0` | `TestIndexedCollisionTriangles` | 46.00% | Work in progress |
| `0x004fb460` | `TestCollisionTriangleArray` | 67.50% | Work in progress |
| `0x004fb4f0` | `SpatialIndexLink::Initialize` | 17.02% | Work in progress |
| `0x004fb550` | `InitializeCollisionQueryBoundsFromEndpoints` | 50.00% | Work in progress |
| `0x004fb610` | `InitializeCollisionQueryBoundsFromRadius` | 8.70% | Work in progress |
| `0x004fb680` | `SpatialIndexLink::Destroy` | 100.00% | Work in progress |
| `0x004fb690` | `SpatialIndexLink::Detach` | 36.00% | Work in progress |
| `0x004fb6e0` | `SpatialIndexLink::RefreshBounds` | 77.78% | Work in progress |
| `0x004fb710` | `SpatialIndexLink::Attach` | 100.00% | Work in progress |
| `0x004fb740` | `SpatialIndexLink::SetFilter` | 75.36% | Work in progress |
| `0x004fb7a0` | `SpatialIndexLink::Update` | 100.00% | Work in progress |
| `0x004fb7f0` | `RendererAuxiliaryResourceRecord::CopyToDeviceMaterial` | 78.87% | Work in progress |
| `0x004fb880` | `ClipGuiRectangle` | 77.94% | Work in progress |
| `0x004fba30` | `CachedCollisionContact::PrepareContact` | 38.36% | Work in progress |
| `0x004fbab0` | `CachedCollisionContact::UpdateContact` | 37.01% | Work in progress |
| `0x004fbe50` | `ContactCacheController::ResetContacts` | 100.00% | Work in progress |
| `0x004fbe60` | `ContactCacheController::CollectContacts` | 40.31% | Work in progress |
| `0x004fbf50` | `ContactCacheController::PrepareContacts` | 87.27% | Work in progress |
| `0x004fbf90` | `ContactCacheController::UpdateContacts` | 87.27% | Work in progress |
| `0x004fc050` | `ComputeSingleBodyContactImpulse` | 19.05% | Work in progress |
| `0x004fc1f0` | `ComputeTwoBodyContactImpulse` | 12.16% | Work in progress |
| `0x004fc4e0` | `ComputePenetrationCorrectionScale` | 35.56% | Work in progress |
| `0x004fc530` | `ComputeContactSeparationWeights` | 100.00% | Verified exact |
| `0x004fc580` | `SeparateContactBodies` | 71.08% | Work in progress |
| `0x004fc680` | `SeparateContactFromStaticBody` | 32.81% | Work in progress |
| `0x00502d10` | `ParseAiffAudioData` | 28.57% | Work in progress |
| `0x00502f00` | `SwapBigEndianDword` | 8.33% | Work in progress |
| `0x00502f20` | `SwapBigEndianWord` | 14.29% | Work in progress |
| `0x00502f40` | `ConvertBigEndianPcm` | 31.58% | Work in progress |
| `0x00502fc0` | `ParseWaveOrAiffAudioData` | 45.59% | Work in progress |
| `0x00503050` | `ParseWaveAudioData` | 18.62% | Work in progress |
| `0x00503340` | `MapGameAudioGain` | 85.00% | Work in progress |
| `0x00503390` | `CopyAudioBytes` | 23.08% | Work in progress |
| `0x005033c0` | `ClearAudioBytes` | 34.62% | Work in progress |
| `0x00505370` | `GetBoxSupportPoint` | 23.46% | Work in progress |
| `0x005054d0` | `GetOpposingBoxSupportPoint` | 24.58% | Work in progress |
| `0x00505630` | `TestBoxAgainstSphereRecord` | 11.72% | Work in progress |
| `0x00505ae0` | `TestOrientedBoxAxes` | 13.56% | Work in progress |
| `0x00506580` | `SelectMinimumBoxAxis` | 30.16% | Work in progress |
| `0x005066f0` | `TestBoxAgainstBoxRecord` | 13.21% | Work in progress |
| `0x005069e0` | `BuildBoxBoundingSphere` | 54.90% | Work in progress |
| `0x00507310` | `SpatialSlotNode::Initialize` | 18.18% | Work in progress |
| `0x00507320` | `SpatialSlotNode::InsertBefore` | 80.00% | Work in progress |
| `0x00507350` | `SpatialSlotNode::InsertAfter` | 45.71% | Work in progress |
| `0x00507380` | `SpatialSlotNode::Unlink` | 100.00% | Verified exact |
| `0x005073a0` | `SpatialSlotAllocator::SpatialSlotAllocator` | 27.03% | Work in progress |
| `0x00507410` | `SpatialSlotAllocator::Reset` | 100.00% | Verified exact |
| `0x00507420` | `SpatialSlotAllocator::AllocateBefore` | 68.97% | Work in progress |
| `0x00507470` | `SpatialSlotAllocator::RemoveValue` | 78.43% | Work in progress |
| `0x005074b0` | `SpatialSlotAllocator::ReleaseNode` | 100.00% | Verified exact |
| `0x005074f0` | `SpatialSlotAllocator::GetNodeIndex` | 100.00% | Verified exact |
| `0x00507520` | `SpatialSlotAllocator::GetNodeValue` | 40.00% | Work in progress |
| `0x00507530` | `FindSpatialSlot` | 100.00% | Verified exact |
| `0x00507550` | `SpatialSlotAllocator::AllocateAfter` | 68.97% | Work in progress |
| `0x005075a0` | `SpatialLinkRegistry::InitializeList` | 58.33% | Work in progress |
| `0x005075d0` | `SpatialLinkRegistry::ClearHead` | 100.00% | Verified exact |
| `0x005075e0` | `SpatialLinkRegistry::InsertValue` | 70.18% | Work in progress |
| `0x00507670` | `SpatialLinkRegistry::UpdateNode` | 78.32% | Work in progress |
| `0x00507720` | `SpatialLinkRegistry::RemoveValue` | 100.00% | Verified exact |
| `0x00507740` | `SpatialLinkRegistry::RemoveNode` | 88.00% | Work in progress |
| `0x00507770` | `SpatialLinkRegistry::GetNodeIndex` | 100.00% | Verified exact |
| `0x00507780` | `SpatialLinkRegistry::GetNodeValue` | 100.00% | Verified exact |
| `0x005078c0` | `SpatialPairList::Initialize` | 75.56% | Work in progress |
| `0x00507910` | `SpatialPairList::AddPair` | 13.02% | Work in progress |
| `0x00507a00` | `SpatialPairList::CanPair` | 36.07% | Work in progress |
| `0x00507a80` | `SpatialPairList::ProcessCommonPairs` | 18.63% | Work in progress |
| `0x00507be0` | `SpatialAxisIndex::InitializeFirst` | 91.67% | Work in progress |
| `0x00507ca0` | `SpatialAxisIndex::Remove` | 100.00% | Verified exact |
| `0x00507cb0` | `SpatialAxisIndex::InitializeSecond` | 91.67% | Work in progress |
| `0x00507d10` | `DynamicSpatialIndex::DynamicSpatialIndex` | 100.00% | Verified exact |
| `0x00507d60` | `DynamicSpatialIndex::ResetQuery` | 89.47% | Work in progress |
| `0x00507dc0` | `IntersectCollisionCandidateArrays` | 20.59% | Work in progress |
| `0x00507e10` | `DynamicSpatialIndex::CollectCandidates` | 97.06% | Work in progress |
| `0x00507e60` | `DynamicSpatialIndex::IntersectSegment` | 100.00% | Verified exact |
| `0x00507ef0` | `BuildXAxisCollisionPairs` | 6.56% | Work in progress |
| `0x00507f80` | `BuildZAxisCollisionPairs` | 6.56% | Work in progress |
| `0x00508010` | `SpatialAxisIndex::UpdateFirst` | 1.87% | Work in progress |
| `0x00508100` | `SpatialAxisIndex::UpdateSecond` | 1.87% | Work in progress |
| `0x005081f0` | `CollectZAxisCandidates` | 8.33% | Work in progress |
| `0x00508290` | `CollectXAxisCandidates` | 8.33% | Work in progress |
| `0x00508330` | `SpatialAxisIndex::InsertFirst` | 4.82% | Work in progress |
| `0x005083e0` | `SpatialAxisIndex::InsertSecond` | 4.82% | Work in progress |
| `0x005084b0` | `SpatialGridCell::Initialize` | 40.00% | Work in progress |
| `0x005084c0` | `SpatialGridCell::Insert` | 100.00% | Verified exact |
| `0x005084e0` | `SpatialGridCell::Remove` | 100.00% | Verified exact |
| `0x00508500` | `StaticSpatialGrid::GetCell` | 94.12% | Work in progress |
| `0x00508520` | `CollectGridCollisionCandidates` | 18.11% | Work in progress |
| `0x00508690` | `StaticSpatialGrid::StaticSpatialGrid` | 71.07% | Work in progress |
| `0x00508740` | `StaticSpatialGrid::Insert` | 28.26% | Work in progress |
| `0x00508840` | `StaticSpatialGrid::Remove` | 88.89% | Work in progress |
| `0x00508890` | `StaticSpatialGrid::Update` | 100.00% | Verified exact |
| `0x005088b0` | `StaticSpatialGrid::PrepareQuery` | 74.51% | Work in progress |
| `0x005088f0` | `StaticSpatialGrid::IntersectSegment` | 97.44% | Work in progress |
| `0x00508970` | `VisitCollisionSegmentCell` | 98.00% | Work in progress |
| `0x00508a10` | `TryRecordCollisionPair` | 20.71% | Work in progress |
| `0x00508ca0` | `TryAppendBoxContact` | 55.38% | Work in progress |
| `0x00508da0` | `CollisionTriangleCandidates::CollisionTriangleCandidates` | 31.25% | Work in progress |
| `0x00508e10` | `ReleaseTriangleCandidates` | 60.00% | Work in progress |
| `0x00509600` | `AppendSphereSphereContact` | 17.39% | Work in progress |
| `0x00509760` | `IntersectRayWithCollisionSphere` | 57.24% | Work in progress |
| `0x00509a40` | `TransformCollisionSphereRecord` | 89.47% | Work in progress |
| `0x0050a110` | `CollisionTriangleCandidates::AddRange` | 5.78% | Work in progress |
| `0x0050a710` | `CollisionTriangleCandidates::Insert` | 19.54% | Work in progress |
| `0x0050a810` | `CollisionTriangleCandidates::AppendRange` | 63.16% | Work in progress |
| `0x0050a890` | `CollisionTreeQueue::AdvanceWrite` | 100.00% | Verified exact |
| `0x0050ad20` | `IntersectCollisionTreeSegment` | 91.67% | Work in progress |
| `0x0050ad60` | `IntersectSegmentBounds` | 18.08% | Work in progress |
| `0x0050b090` | `CollectCollisionSegmentTriangles` | 30.55% | Work in progress |
| `0x0050b380` | `GetCollisionChildHalfExtent` | 12.77% | Work in progress |
| `0x0050bfd0` | `IntersectRayWithCollisionTriangle` | 10.21% | Work in progress |
