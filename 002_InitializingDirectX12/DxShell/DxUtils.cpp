#include "DxShell.h"

#include <comdef.h>


void DxShell::ClearFrameBuffer ( DirectX::XMFLOAT4 bg_color ) {
    int cur_buf = swapChain->GetCurrentBackBufferIndex ( );
    D3D12_RESOURCE_BARRIER from_present_to_rt = {};
    from_present_to_rt.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    from_present_to_rt.Transition.pResource = swapChainBuffers[cur_buf].Get ( );
    from_present_to_rt.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    from_present_to_rt.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

    cmdList->ResourceBarrier ( 1, &from_present_to_rt );

    cmdList->RSSetViewports ( 1, &viewport );
    cmdList->RSSetScissorRects ( 1, &scissorsRect );
    cmdList->OMSetRenderTargets ( 1, &rtvCpuHandles[cur_buf],
                                  0, &dsvCpuHandle );
    cmdList->ClearRenderTargetView ( rtvCpuHandles[cur_buf],
                                     reinterpret_cast<FLOAT*>(&bg_color),
                                     1, &scissorsRect );
    cmdList->ClearDepthStencilView ( dsvCpuHandle, D3D12_CLEAR_FLAG_DEPTH,
                                     1.0f, 0, 1, &scissorsRect );
}


void DxShell::LogError ( HRESULT result, std::string error ) {
    lastError = "Failed to create DXGI factory, HRESULT = ";
    lastError += _com_error ( result ).ErrorMessage ( );
    OutputDebugString ( (lastError + "\n").data ( ) );
}


void DxShell::SetSwapChainToPresentState ( ) {
    int cur_buf = swapChain->GetCurrentBackBufferIndex ( );
    D3D12_RESOURCE_BARRIER from_rt_to_present = {};
    from_rt_to_present.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    from_rt_to_present.Transition.pResource   = swapChainBuffers[cur_buf].Get ( );
    from_rt_to_present.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    from_rt_to_present.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    cmdList->ResourceBarrier ( 1, &from_rt_to_present );
}

void DxShell::Execute ( ) {
    cmdList->Close ( );
    ComPtr<ID3D12CommandList> temp_cmdList = cmdList;
    cmdQueue->ExecuteCommandLists ( 1, temp_cmdList.GetAddressOf ( ) );
    cmdQueue->Signal ( fence.Get ( ), fenceValue );
    fence->SetEventOnCompletion ( fenceValue, fenceEvent );
    WaitForSingleObject ( fenceEvent, INFINITE );
    ++fenceValue;
    cmdAllocator->Reset ( );
    cmdList->Reset ( cmdAllocator.Get ( ), nullptr );
}

void DxShell::ShowCurrentFrame ( ) {
    swapChain->Present ( 0, 0 );
}
