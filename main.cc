#include <iostream>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#define FRAMECOUNT 2

D3D12_VIEWPORT viewport;
D3D12_RECT scissorRect;
ComPtr<IDXGISwapChain3> swapChain;
ComPtr<ID3D12Device> device;
ComPtr<ID3D12Resource> renderTargets[FRAMECOUNT];
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<ID3D12RootSignature> rootSignature;
ComPtr<ID3D12DescriptorHeap> rtvHeap;
ComPtr<ID3D12PipelineState> pipelineState;
ComPtr<ID3D12GraphicsCommandList> commandList;
UINT rtvDescriptorSize, frameIndex;

inline void tif(HRESULT hr) {
	if (FAILED(hr))
	throw std::exception();
}

void initD3D(UINT HEIGHT, UINT WIDTH) {
	/* Create new device */
	ComPtr<IDXGIFactory4> factory;
	tif(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

	/* Describe and create the command queue */
	D3D12_COMMAND_QUEUE_DESC qdsc = {};
	qdsc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	qdsc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	tif(device->CreateCommandQueue(&qdsc, IID_PPV_ARGS(&commandQueue)));

	/* Describe and create swap chain */
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = FRAMECOUNT;
	scd.BufferDesc.Height = HEIGHT;
	scd.BufferDesc.Width = WIDTH;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.OutputWindow = GetActiveWindow();
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;

	ComPtr<IDXGISwapChain> sc;
	tif(factory->CreateSwapChain(commandQueue.Get(),
						&scd,
						&sc));
	tif(sc.As(&swapChain));

	tif(factory->MakeWindowAssociation(GetActiveWindow(),
							DXGI_MWA_NO_ALT_ENTER));
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	/* Create descriptor heaps */
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvhd = {};
		rtvhd.NumDescriptors = FRAMECOUNT;
		rtvhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tif(device->CreateDescriptorHeap(&rtvhd, IID_PPV_ARGS(&rtvHeap)));

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

#if 0
	/* Create frame resources */
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT n = 0; n < FRAMECOUNT; n++) {
			tif(swapChain->GetBuffer(n, IID_PPV_ARGS(renderTargets[n])));
			device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}
	}
	tif(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
#endif
}

int main() {
	initD3D(1280, 720);

	return 0;
}
