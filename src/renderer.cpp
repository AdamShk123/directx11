#include "./renderer.hpp"

using namespace DirectX;

namespace Renderer 
{

Renderer::Renderer(Window& window) : m_window(window)
{
    createFactory();
    auto bestAdapter = findBestAdapter();
    createDevice(bestAdapter);

    #if defined(_DEBUG)
    DX::ThrowIfFailed(m_device->QueryInterface(IID_PPV_ARGS(&m_debugController)));
    #endif // _DEBUG

    createSwapChain();
    createRenderViewTarget();
    createDepthStencilBuffer();
    createDepthStencilView();
    createVertexBuffer();
    createIndexBuffer();
    createConstantBuffer();

    #if defined(_DEBUG)
    unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #else
    unsigned int compileFlags = 0;
    #endif

    //std::cerr << "\033[31m" << "testing error" << "\033[0m" << std::endl;

    std::wstring vertexShaderPath{ L"./vertex.hlsl" };

    compileVertexShader(vertexShaderPath, compileFlags);

    createInputLayout();

    createDepthStencilState();

    std::wstring pixelShaderPath{ L"./pixel.hlsl" };

    compilePixelShader(pixelShaderPath, compileFlags);

    createRasterizerState();

    populateConstantBufferDataStruct();
}

Renderer::~Renderer() 
{
    if (m_factory) m_factory->Release();
    if (m_device) m_device->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_backBuffer) m_backBuffer->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_constantBuffer) m_constantBuffer->Release();
    if (m_rasterState) m_rasterState->Release();
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_shaderBlob) m_shaderBlob->Release();
    if (m_shaderErrors) m_shaderErrors->Release();
    if (m_vertexLayout) m_vertexLayout->Release();
    if (m_depthStencilBuffer) m_depthStencilBuffer->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_depthStencilState) m_depthStencilState->Release();

    #if defined(_DEBUG)
    if(m_debugController) m_debugController->Release();
    #endif
}

void Renderer::render() 
{
    D3D11_VIEWPORT viewport{};
    viewport.Width = static_cast<float>(CONSTANTS::SCREEN_WIDTH);
    viewport.Height = static_cast<float>(CONSTANTS::SCREEN_HEIGHT);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    // Set the position of the constant buffer in the vertex shader.
    unsigned int bufferNumber = 0;
    // Lock the constant buffer so it can be written to.

    D3D11_MAPPED_SUBRESOURCE mappedResource{};
    DX::ThrowIfFailed(m_deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD,
        0, &mappedResource));
    // Get a pointer to the data in the constant buffer.
    memcpy(mappedResource.pData, &m_constantBufferData, sizeof(ConstantBuffer));
    // Unlock the constant buffer.
    m_deviceContext->Unmap(m_constantBuffer, 0);
    // Finally set the constant buffer in the vertex shader with the updated
    // values.
    m_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

    m_deviceContext->RSSetViewports(1, &viewport);

    m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::AntiqueWhite);

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(
        m_depthStencilView,
        D3D11_CLEAR_DEPTH,
        1.0f,
        0
    );

    m_deviceContext->IASetInputLayout(m_vertexLayout);

    m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_deviceContext->DrawIndexed(static_cast<unsigned int>(INDEX_BUFFER_DATA.size()), 0, 0);

    m_swapChain->Present(0, 0);
}

void Renderer::createFactory()
{
    auto createFactoryResult = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));

    DX::ThrowIfFailed(createFactoryResult);
}

IDXGIAdapter* Renderer::findBestAdapter() noexcept
{
    IDXGIAdapter* adapter = nullptr;
    IDXGIAdapter* bestAdapter = nullptr;

    size_t bestAdapterMemory = 0;

    int numAdapters = 0;

    while(true) 
    {
        if (FAILED(m_factory->EnumAdapters(numAdapters, &adapter))) break;

        numAdapters += 1;

        DXGI_ADAPTER_DESC adapterDesc{};
        if (FAILED(adapter->GetDesc(&adapterDesc))) 
        {
            adapter->Release();
            continue;
        }

        if (adapterDesc.DedicatedVideoMemory > bestAdapterMemory) 
        {
            if (bestAdapter) 
            {
                bestAdapter->Release();
            }

            bestAdapterMemory = adapterDesc.DedicatedVideoMemory;
            bestAdapter = adapter;
        }
        else 
        {
            adapter->Release();
        }
    }

    return bestAdapter;
}

void Renderer::createDevice(IDXGIAdapter* adapter)
{
    std::array<D3D_FEATURE_LEVEL, 7> featureLevelInputs{
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL featureLevelOutputs = featureLevelInputs[0];

    unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

    #if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif // _DEBUG


    auto createDeviceResult = D3D11CreateDevice(
        adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        0,
        flags,
        featureLevelInputs.data(),
        7,
        D3D11_SDK_VERSION,
        &m_device,
        &featureLevelOutputs,
        &m_deviceContext
    );

    DX::ThrowIfFailed(createDeviceResult);
}

void Renderer::createSwapChain()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = CONSTANTS::SCREEN_WIDTH;
    swapChainDesc.BufferDesc.Height = CONSTANTS::SCREEN_HEIGHT;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = m_window.getHandle();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    auto createSwapChainResult = m_factory->CreateSwapChain(
        m_device,
        &swapChainDesc,
        &m_swapChain
    );

    DX::ThrowIfFailed(createSwapChainResult);
}

void Renderer::createRenderViewTarget() 
{
    auto getBufferResult = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer));

    DX::ThrowIfFailed(getBufferResult);

    auto createRenderTargetViewResult = m_device->CreateRenderTargetView(m_backBuffer, nullptr, &m_renderTargetView);

    DX::ThrowIfFailed(createRenderTargetViewResult);
}

void Renderer::createVertexBuffer()
{
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<unsigned int>(sizeof(Vertex) * VERTEX_BUFFER_DATA.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA bufferData{};
    bufferData.pSysMem = (void*)VERTEX_BUFFER_DATA.data();
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    auto createBufferResult = m_device->CreateBuffer(
        &bufferDesc,
        &bufferData,
        &m_vertexBuffer
    );

    DX::ThrowIfFailed(createBufferResult);
}

void Renderer::createIndexBuffer()
{
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<unsigned int>(sizeof(unsigned int) * INDEX_BUFFER_DATA.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA bufferData{};
    bufferData.pSysMem = (void*)INDEX_BUFFER_DATA.data();
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    auto createBufferResult = m_device->CreateBuffer(
        &bufferDesc,
        &bufferData,
        &m_indexBuffer
    );

    DX::ThrowIfFailed(createBufferResult);
}

void Renderer::compileVertexShader(const std::wstring& path, unsigned int compileFlags)
{
    auto compileShaderResult = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        compileFlags,
        0,
        &m_shaderBlob,
        &m_shaderErrors
    );

    try
    {
        DX::ThrowIfFailed(compileShaderResult);
    }
    catch (DX::com_exception e)
    {
        std::println("erorr: {}", m_shaderErrors->GetBufferPointer());
        throw e;
    }

    auto createShaderResult = m_device->CreateVertexShader(
        m_shaderBlob->GetBufferPointer(),
        m_shaderBlob->GetBufferSize(),
        nullptr,
        &m_vertexShader
    );

    DX::ThrowIfFailed(createShaderResult);
}

void Renderer::compilePixelShader(const std::wstring& path, unsigned int compileFlags)
{
    auto compileShaderResult = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &m_shaderBlob,
        &m_shaderErrors
    );

    try
    {
        DX::ThrowIfFailed(compileShaderResult);
    }
    catch (DX::com_exception e)
    {
        std::println("erorr: {}", m_shaderErrors->GetBufferPointer());
        throw e;
    }

    auto createShaderResult = m_device->CreatePixelShader(
        m_shaderBlob->GetBufferPointer(),
        m_shaderBlob->GetBufferSize(),
        nullptr,
        &m_pixelShader
    );

    DX::ThrowIfFailed(createShaderResult);
}

void Renderer::createInputLayout()
{
    // Define the input layout
    std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout{};

    layout[0].SemanticName = "POSITION";
    layout[0].SemanticIndex = 0;
    layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[0].InputSlot = 0;
    layout[0].AlignedByteOffset = 0;
    layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[0].InstanceDataStepRate = 0;

    layout[1].SemanticName = "COLOR";
    layout[1].SemanticIndex = 0;
    layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[1].InputSlot = 0;
    layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[1].InstanceDataStepRate = 0;

    // Create the input layout
    auto createInputLayout = m_device->CreateInputLayout(
        layout.data(),
        static_cast<unsigned int>(layout.size()),
        m_shaderBlob->GetBufferPointer(),
        m_shaderBlob->GetBufferSize(),
        &m_vertexLayout
    );

    DX::ThrowIfFailed(createInputLayout);
}

void Renderer::createRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    DX::ThrowIfFailed(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState));
    m_deviceContext->RSSetState(m_rasterState);
}

void Renderer::createDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC depthBufferDesc{};
    depthBufferDesc.Width = CONSTANTS::SCREEN_WIDTH;
    depthBufferDesc.Height = CONSTANTS::SCREEN_HEIGHT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    auto createStencilBufferResult = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
    DX::ThrowIfFailed(createStencilBufferResult);
}

void Renderer::createDepthStencilView()
{
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    auto createDepthStencilViewResult = m_device->CreateDepthStencilView(
        m_depthStencilBuffer,
        &depthStencilViewDesc,
        &m_depthStencilView
    );
    DX::ThrowIfFailed(createDepthStencilViewResult);
}

void Renderer::createDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    auto createDepthStencilStateResult = m_device->CreateDepthStencilState(
        &depthStencilDesc,
        &m_depthStencilState
    );
    DX::ThrowIfFailed(createDepthStencilStateResult);
    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void Renderer::populateConstantBufferDataStruct()
{
    m_constantBufferData.model = SimpleMath::Matrix().CreateTranslation(SimpleMath::Vector3(0.5f, 0.0f, 0.0f));

    m_constantBufferData.view = SimpleMath::Matrix::CreateLookAt(
        SimpleMath::Vector3(0.0f, 0.0f, 2.0f),
        SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
        SimpleMath::Vector3(0.0f, 1.0f, 0.0f)
    );

    m_constantBufferData.projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        90.0f,
        static_cast<float>(CONSTANTS::SCREEN_WIDTH) / static_cast<float>(CONSTANTS::SCREEN_HEIGHT),
        0.1f,
        100.0f
    );
}

void Renderer::createConstantBuffer()
{
    D3D11_BUFFER_DESC constantBufferDesc{};
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;

    auto createConstantBufferResult = m_device->CreateBuffer(
        &constantBufferDesc,
        nullptr,
        &m_constantBuffer
    );
    DX::ThrowIfFailed(createConstantBufferResult);
}

}