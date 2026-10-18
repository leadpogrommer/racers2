# Decompilation progress

The primary milestone is a playable rebuild. Instruction accuracy remains
visible, but does not take precedence over startup and gameplay dependencies.
Reviewed semantically identical near matches (initially at least 95% raw) are
implemented functions, counted separately from exact matches.

Latest combined Python/SP5/Wine and CMake/NMake verification: 2026-10-18.
The match guard verified **253 exact functions and ten reviewed near
matches**. Another 332 reconstructions remain `STUB`; both builders agree on
all 595 measured raw scores and annotation statuses. Data comparison verifies
122 annotated globals with zero issues. Unlinked inventory entries and folded
aliases may also appear in the CLI denominator without another measured row.

This is the reconstructed subset, not whole-game coverage. The automatic
Ghidra inventory has **4,254 discovered function starts**, including **21 thunks**
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
| `0x004a2dd0` | `GetGameManager` | 93.33% | Work in progress |
| `0x004a3170` | `GameManager::ResizeObjectRegistry` | 23.75% | Work in progress |
| `0x004a3290` | `GameManager::RegisterObject` | 45.31% | Work in progress |
| `0x004a3350` | `GameManager::UnregisterObject` | 72.22% | Work in progress |
| `0x004a33d0` | `GameManager::SortObjectsByParent` | 28.78% | Work in progress |
| `0x004a34a0` | `GameManager::RebuildObjectWorkLists` | 35.38% | Work in progress |
| `0x004a3580` | `GameManager::InitializeRegisteredObjects` | 85.71% | Work in progress |
| `0x004a36c0` | `GameManager::UpdateObjects` | 47.27% | Work in progress |
| `0x004a3710` | `GameManager::RemoveAllObjects` | 23.16% | Work in progress |
| `0x004a3810` | `SimulationScheduler::Reset` | 72.73% | Work in progress |
| `0x004a3840` | `SimulationScheduler::NeedsSimulationStep` | 66.46% | Work in progress |
| `0x004a3ca0` | `ShutdownGameManager` | 100.00% | Verified exact |
| `0x004a3ed0` | `GetMemoryHeap` | 60.00% | Work in progress |
| `0x004a3f20` | `LookupNoOpCleanup` | 100.00% | Verified exact |
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
| `0x004a5e90` | `InitializeSimulationFrameRate` | 53.33% | Work in progress |
| `0x004a5ec0` | `InitializeSimulationStepSeconds` | 100.00% | Verified exact |
| `0x004a5ef0` | `InitializeSimulationStepMilliseconds` | 100.00% | Verified exact |
| `0x004a6190` | `ResetGameObjectInitializationValue` | 63.16% | Work in progress |
| `0x004a74b0` | `IsLookupPointWithinRadius` | 45.45% | Work in progress |
| `0x004a7510` | `ApplyLookupResourceState` | 18.70% | Work in progress |
| `0x004a7cc0` | `?ReleaseRendererDrawPacket@@YIXPAURendererDrawPacket@?%Z:\home\ilya\work\lr2_decomp\racers2\LEGO Racers 2\include\applicationdispatch.h1817512615@@PAX@Z` | 66.67% | Work in progress |
| `0x004a9d20` | `GetLookupResourceOrigin` | 91.30% | Work in progress |
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
| `0x004b7b80` | `InputController::InputController` | 83.33% | Work in progress |
| `0x004b7c20` | `InputController::~InputController` | 100.00% | Verified exact |
| `0x004b7c50` | `InputControllerManager::InitializeTiming` | 94.12% | Work in progress |
| `0x004b7c70` | `InputControllerManager::ResetRecording` | 28.38% | Work in progress |
| `0x004b7d40` | `InputControllerManager::Shutdown` | 98.18% | Reviewed near match |
| `0x004b7d80` | `InputController::SetDevice` | 100.00% | Verified exact |
| `0x004b7da0` | `InputController::AppendBinding` | 100.00% | Verified exact |
| `0x004b7dd0` | `GetControllerInputDevice` | 89.66% | Work in progress |
| `0x004b7e30` | `InputController::GetBinding` | 100.00% | Verified exact |
| `0x004b7e40` | `InputController::GetRecordedInput` | 71.11% | Work in progress |
| `0x004b7eb0` | `InputController::GetCurrentInput` | 65.57% | Work in progress |
| `0x004b7f20` | `InputController::RecordInputRange` | 92.00% | Work in progress |
| `0x004b7f60` | `InputControllerManager::FindOrCreateController` | 48.23% | Work in progress |
| `0x004b8460` | `InputControllerManager::GetTimingState` | 34.85% | Work in progress |
| `0x004b8520` | `InputControllerManager::GetElapsedTime` | 45.45% | Work in progress |
| `0x004b8570` | `InputControllerManager::GetCurrentFrame` | 100.00% | Verified exact |
| `0x004b85a0` | `InputControllerManager::PollInput` | 47.84% | Work in progress |
| `0x004b86f0` | `InputControllerManager::PollController` | 11.13% | Work in progress |
| `0x004b93e0` | `InitializeInputControllerManagerStorage` | 66.67% | Work in progress |
| `0x004b94c0` | `InputControllerManager::AcquirePollMutex` | 100.00% | Work in progress |
| `0x004b94e0` | `InputControllerManager::ReleasePollMutex` | 100.00% | Verified exact |
| `0x004b9500` | `InputControllerManager::InitializePolling` | 100.00% | Verified exact |
| `0x004b9560` | `InputControllerManager::StopPolling` | 69.84% | Work in progress |
| `0x004b95b0` | `ControllerPollTimerCallback` | 100.00% | Verified exact |
| `0x004bb1f0` | `SetScreenshotDirectory` | 100.00% | Verified exact |
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
| `0x004c11e0` | `RendererAuxiliaryResourceRecord::InitializeMaterial` | 19.77% | Work in progress |
| `0x004c32b0` | `FUN_004c32b0` | 100.00% | Verified exact |
| `0x004c33b0` | `FUN_004c33b0` | 100.00% | Verified exact |
| `0x004c3460` | `FindFirstNonemptyRow` | 100.00% | Verified exact |
| `0x004c3d70` | `BitmapImage::BitmapImage` | 100.00% | Verified exact |
| `0x004c3db0` | `BitmapFont::GetGlyphMetrics` | 100.00% | Verified exact |
| `0x004c3f40` | `Terrain::GetCellVertices` | 63.61% | Work in progress |
| `0x004c4140` | `Terrain::GetMinimumBounds` | 100.00% | Verified exact |
| `0x004c41a0` | `Terrain::GetMaximumBounds` | 100.00% | Verified exact |
| `0x004c6980` | `RenderInitializedResource` | 15.09% | Work in progress |
| `0x004c6fe0` | `RenderDeferredResource` | 20.66% | Work in progress |
| `0x004cb340` | `TerrainVertexGrid::SelectEdges` | 100.00% | Verified exact |
| `0x004cb530` | `UpdateTerrainLodMetrics` | 56.74% | Work in progress |
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
| `0x004d42e0` | `TerrainVisibilityContext::GetCellState` | 9.72% | Work in progress |
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
| `0x004da9f0` | `?ReadBitmapInfoSettings@@YAIPBDPAUBitmapInfoSettings@?%Z:\home\ilya\work\lr2_decomp\racers2\LEGO Racers 2\include\bitmapimage.h514413611@@@Z` | 2.29% | Work in progress |
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
| `0x004e7b10` | `InitializeRendererFloatingPoint` | 71.43% | Work in progress |
| `0x004e7b30` | `SaveFloatingPointControl` | 100.00% | Verified exact |
| `0x004e7b40` | `RestoreFloatingPointControl` | 100.00% | Verified exact |
| `0x004e7b50` | `ShowDiagnosticDialog` | 100.00% | Verified exact |
| `0x004e7b90` | `AllocateAlignedMemory` | 100.00% | Verified exact |
| `0x004e7bb0` | `DecompressGtcBlock` | 9.78% | Work in progress |
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
| `0x004f4a40` | `InitializeMultimediaTimerService` | 53.01% | Work in progress |
| `0x004f4ad0` | `ShutdownMultimediaTimerService` | 100.00% | Verified exact |
| `0x004f4af0` | `MultimediaTimerService::EndPeriod` | 100.00% | Verified exact |
| `0x004f4b10` | `MultimediaTimerService::CreatePeriodicEvent` | 100.00% | Verified exact |
| `0x004f4b40` | `MultimediaTimerService::KillPeriodicEvent` | 100.00% | Verified exact |
| `0x004f4b60` | `MultimediaTimerService::CreateNamedMutex` | 100.00% | Verified exact |
| `0x004f4b90` | `MultimediaTimerService::CloseMutex` | 100.00% | Verified exact |
| `0x004f4bb0` | `MultimediaTimerService::AcquireNamedMutex` | 67.80% | Work in progress |
| `0x004f4c00` | `MultimediaTimerService::ReleaseNamedMutex` | 100.00% | Verified exact |
| `0x004f54a0` | `BitmapImage::LoadTga` | 17.27% | Work in progress |
| `0x004f59c0` | `BitmapImage::ReleaseStorage` | 95.24% | Reviewed near match |
| `0x004f5a10` | `WriteBitmapTgaFile` | 30.77% | Work in progress |
| `0x004f6310` | `InitializePlatformServices` | 70.83% | Work in progress |
| `0x004fb7f0` | `RendererAuxiliaryResourceRecord::CopyToDeviceMaterial` | 78.87% | Work in progress |
| `0x004fb880` | `ClipGuiRectangle` | 77.94% | Work in progress |
