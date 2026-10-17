# Decompilation progress

The primary milestone is a playable rebuild. Instruction accuracy remains
visible, but does not take precedence over startup and gameplay dependencies.
Reviewed semantically identical near matches (initially at least 95% raw) are
implemented functions, counted separately from exact matches.

Latest combined Python/SP5/Wine and CMake/NMake verification: 2026-10-17.
The match guard verified **150 exact functions and four reviewed near
matches**; 99 additional reconstructions remain `STUB` because behavior is
unresolved or below the near-match threshold. Both builds agree on all 253
measured rows and annotation statuses. Data comparison verifies 64 annotated
globals with zero issues. The CLI denominator of 256 also includes three
inventory-only folded/cleanup functions that have no additional measured
match rows.

This is the reconstructed subset, not whole-game coverage. The automatic
Ghidra inventory has **4,228 discovered function starts**, including **21 thunks**
and linked runtime/library code. It is incomplete and is not a verified count
of game-authored functions; see [inventory notes](research.md#automatic-function-inventory).
Identified libraries are excluded from these matches with `--nolib`.
The required memory pool and archive-loading reconstruction are now included
in the build; see the [runtime roadmap](runtime-roadmap.md) and
[library identification](library-identification.md).

| Original VA | Function | Instruction accuracy | Status |
| --- | --- | ---: | --- |
| `0x00403910` | `IsApplicationDevelopmentMode` | 100.00% | Verified exact |
| `0x00403930` | `OnApplicationDiagnostic` | 100.00% | Verified exact |
| `0x0040a7d0` | `BuildUserDataPath` | 100.00% | Verified exact |
| `0x0040ec90` | `Application::GetDisplaySettings` | 100.00% | Verified exact |
| `0x0045dcb0` | `FUN_0045dcb0` | 100.00% | Verified exact |
| `0x0045dda0` | `IsTokenCharacter` | 100.00% | Verified exact |
| `0x0045de00` | `IsHorizontalSpace` | 100.00% | Verified exact |
| `0x0045de20` | `FUN_0045de20` | 57.94% | Work in progress |
| `0x0045df40` | `FUN_0045df40` | 100.00% | Verified exact |
| `0x0045dfd0` | `FUN_0045dfd0` | 100.00% | Verified exact |
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
| `0x004a2ba0` | `FileManager::Read` | 87.38% | Work in progress |
| `0x004a2c20` | `FileManager::Close` | 100.00% | Verified exact |
| `0x004a2c40` | `FileManager::GetSize` | 100.00% | Verified exact |
| `0x004a2c60` | `FileManager::Exists` | 62.30% | Work in progress |
| `0x004a2d20` | `FileManager::SeekCurrent` | 79.49% | Work in progress |
| `0x004a2d80` | `FileManager::SetFlags` | 100.00% | Verified exact |
| `0x004a3ed0` | `GetMemoryHeap` | 60.00% | Work in progress |
| `0x004a3f20` | `OnSavedDisplayConfigExit` | 100.00% | Verified exact |
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
| `0x004a5dc0` | `RoundUpToPowerOfTwo` | 100.00% | Verified exact |
| `0x004a5df0` | `CountTrailingZeroBits` | 100.00% | Verified exact |
| `0x004a5e10` | `BuildSineTable` | 100.00% | Verified exact |
| `0x004ab2a0` | `ReportFatalError` | 85.25% | Work in progress |
| `0x004ab3b0` | `PrintDiagnostic` | 79.41% | Work in progress |
| `0x004abe90` | `RendererZeroPair::RendererZeroPair` | 100.00% | Verified exact |
| `0x004abeb0` | `RendererSubrecordGroup::RendererSubrecordGroup` | 100.00% | Verified exact |
| `0x004ac120` | `DisplayRenderer::UsesHardwareVertexProcessing` | 100.00% | Verified exact |
| `0x004acd00` | `DisplayRenderer::ConfigureNvidiaWorkaround` | 12.61% | Work in progress |
| `0x004acfb0` | `DisplayRenderer::CreateVertexBuffers` | 95.33% | Reviewed near match |
| `0x004ad2c0` | `DisplayRenderer::EnumerateSupportedAdapters` | 33.33% | Work in progress |
| `0x004ad540` | `GetDisplayFormatBitDepth` | 29.17% | Work in progress |
| `0x004ad5a0` | `DisplayRenderer::GetSelectedAdapterModeCount` | 100.00% | Verified exact |
| `0x004ad5c0` | `DisplayRenderer::GetSelectedAdapterMode` | 16.47% | Work in progress |
| `0x004ada60` | `DisplayRenderer::FindBestDisplayMode` | 22.32% | Work in progress |
| `0x004adba0` | `DisplayRenderer::InitializeDefaultRenderStates` | 60.53% | Work in progress |
| `0x004adfe0` | `DisplayRenderer::SetCachedRenderState` | 100.00% | Verified exact |
| `0x004ae010` | `DisplayRenderer::ResetTextureStages` | 30.95% | Work in progress |
| `0x004ae310` | `DisplayRenderer::SetCachedTextureStageState` | 43.48% | Work in progress |
| `0x004aebb0` | `DisplayRenderer::SelectDepthStencilFormat` | 60.38% | Work in progress |
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
| `0x004bb1f0` | `SetScreenshotDirectory` | 100.00% | Verified exact |
| `0x004bd5f0` | `RendererLookupTableState::Initialize` | 51.97% | Work in progress |
| `0x004bd6d0` | `RendererLookupTableState::ReleaseStorage` | 66.67% | Work in progress |
| `0x004bd710` | `RendererLookupTableState::Clear` | 28.07% | Work in progress |
| `0x004bd770` | `RendererLookupTableState::Insert` | 60.00% | Work in progress |
| `0x004c32b0` | `FUN_004c32b0` | 100.00% | Verified exact |
| `0x004c33b0` | `FUN_004c33b0` | 100.00% | Verified exact |
| `0x004c3460` | `FindFirstNonemptyRow` | 100.00% | Verified exact |
| `0x004c3d70` | `BitmapImage::BitmapImage` | 100.00% | Verified exact |
| `0x004c3db0` | `BitmapFont::GetGlyphMetrics` | 100.00% | Verified exact |
| `0x004c3f40` | `Terrain::GetCellVertices` | 63.61% | Work in progress |
| `0x004c4140` | `Terrain::GetMinimumBounds` | 100.00% | Verified exact |
| `0x004c41a0` | `Terrain::GetMaximumBounds` | 100.00% | Verified exact |
| `0x004cb340` | `TerrainVertexGrid::SelectEdges` | 100.00% | Verified exact |
| `0x004cb630` | `Terrain::GetVertex` | 100.00% | Verified exact |
| `0x004cb680` | `Terrain::GetTile` | 100.00% | Verified exact |
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
| `0x004dc590` | `DisplayRenderer::SetTextureFiltering` | 45.95% | Work in progress |
| `0x004e7b10` | `InitializeRendererFloatingPoint` | 71.43% | Work in progress |
| `0x004e7b30` | `SaveFloatingPointControl` | 100.00% | Verified exact |
| `0x004e7b40` | `RestoreFloatingPointControl` | 100.00% | Verified exact |
| `0x004e7b50` | `ShowDiagnosticDialog` | 100.00% | Verified exact |
| `0x004e7b90` | `AllocateAlignedMemory` | 100.00% | Verified exact |
| `0x004e7bb0` | `DecompressGtcBlock` | 9.78% | Work in progress |
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

The latest input batch adds 58 measured reconstructions (33 exact matches and
25 below the annotation threshold), the folded null helper, the complete
service initializer and joystick dispatch table. All 307 input-format entries
match retail, including its mouse button GUID filters. The temporary bootstrap
still does not call input initialization; see [service recovery](work-input-service.md).

The recovered subset includes the application singleton, complete 38-slot dispatch
tables, resource linkage, memory pool, real aligned GlobalAlloc wrapper,
diagnostics, mode-3 file manager and compressed archive reader, plus renderer
and DirectInput helpers. Binary wiring verification passes for the 76
application callback pointers and all 80 device/service pointers. This verifies
table contents, not method-body equivalence or C++ vtable matching.
Raw scores remain visible for unfinished bodies. The four reviewed near matches
are `WinFile::Read` (95.24%), `DisplayRenderer::CreateVertexBuffers` (95.33%),
`MemoryHeap::UpdateCursor` (95.83%), and `InputDeviceRecord::GetButtonValue`
(95.24%). Library provenance remains unresolved for the pool and block decoder.

A bounded run of the rebuilt executable exited with code 0 after startup read
the per-user `C:\users\ilya\Documents\Games\LEGO Racers 2\Saved Games\LR2.cfg`
path, both archive metadata files, and opened GAMEDATA.GTC. This reaches
application construction/config loading and archive initialization through
the current process startup path, but the
entry point is still temporary and exits without creating a window or
rendering gameplay. An isolated real-file probe also reads three archive
assets, including a 349,562-byte texture spanning three logical blocks and a
late archive text file. It exercises compressed reads but makes no
pixel-correctness claim. See [validation](validation.md).
Three original extents are inventory-only: 0x004a2550 has no unique
PDB-name mapping, and 0x0049b930 folds with the heap exit callback in the
rebuilt image. The three-byte null-return helper at 0x00473140 also folds
with the existing zero leaf. These do not add measured match rows.

Display config load/save, managed existence, user-data path construction,
native-window helpers, renderer state helpers and DirectInput state readers
are included. Earlier runtime checks construct the application and read its
config and selected archive assets; window creation and rendering remain
unreached. No game or probe ran during the latest renderer/input batch. Future
runtime execution is reserved for a significant milestone expected to render
the menu or enter gameplay. The extracted resource file contains all five
retail resources (7,022 payload bytes) and is consumed by both builders when
present. Renderer builds require the generated DirectX 8 configuration; both builders have
passed with it, and the isolated probe imports D3D8, DInput8 and DSound8.

Evidence and open dependencies are recorded in the [file facade](work-file-facade.md),
[matrix/vector and quaternion notes](work-matrix-helpers.md),
[spline evaluation](work-spline-evaluation.md), [scalar helpers](work-scalar-helpers.md),
[bitmap scanner](work-bitmap-helpers.md), [bitmap font](work-bitmap-font.md),
[terrain](work-terrain.md), [quaternion codec](work-math-helpers.md),
[screenshot](work-screenshot.md), [memory pool](work-memoryheap.md),
[runtime support](work-runtime-support.md), [display config](work-application-config.md),
[application startup](work-application-runtime.md),
[user data](work-userdatapath.md), [native windows](work-nativewindow.md),
[resource extraction](work-resources.md), [DirectX 8](work-directx8.md),
[renderer lifecycle](work-renderer-lifecycle.md),
[renderer state](work-renderer-state.md), [input devices](work-input-device.md),
[input dispatch](work-input-runtime.md), [input service](work-input-service.md),
[joystick axes](work-joystick-axis.md), [joystick effects](work-joystick-effect.md),
[periodic parameters](work-joystick-periodic.md), [joystick queries](work-joystick-queries.md),
[input setup](work-input-setup.md),
[input queries](work-input-queries.md), [input lifecycle](work-input-lifecycle.md),
and [display startup](work-renderer-device.md) notes.
The [startup research](work-startup.md) and [application-object research](work-application.md)
trace the real entry point and message-loop dependencies. The temporary entry
point still does not start the game.

Open `build/reports/LEGORACERS2.html` for searchable assembly diffs.
Regenerate reports with `python tools/build.py` and `python tools/compare.py`
using the Wine prefix documented in [validation.md](validation.md).
