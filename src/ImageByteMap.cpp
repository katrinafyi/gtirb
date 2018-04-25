#include <gtirb/FilesystemSerialization.hpp>
#include <gtirb/ImageByteMap.hpp>
#include <gtirb/Module.hpp>
#include <gtirb/NodeValidators.hpp>

using namespace gtirb;

BOOST_CLASS_EXPORT_IMPLEMENT(gtirb::ImageByteMap);

ImageByteMap::ImageByteMap() : Node()
{
    this->addParentValidator(gtirb::NodeValidatorHasParentOfType<gtirb::Module>());
    this->addParentValidator(gtirb::NodeValidatorHasNoSiblingsOfType<gtirb::ImageByteMap>());
}

void ImageByteMap::setFileName(boost::filesystem::path x)
{
    this->fileName = x;
}

boost::filesystem::path ImageByteMap::getFileName() const
{
    return this->fileName;
}

void ImageByteMap::setBaseAddress(EA x)
{
    this->baseAddress = x;
}

EA ImageByteMap::getBaseAddress() const
{
    return this->baseAddress;
}

void ImageByteMap::setEntryPointAddress(EA x)
{
    this->entryPointAddress = x;
}

EA ImageByteMap::getEntryPointAddress() const
{
    return this->entryPointAddress;
}

bool ImageByteMap::setEAMinMax(std::pair<gtirb::EA, gtirb::EA> x)
{
    if(x.first <= x.second)
    {
        this->eaMinMax = std::move(x);
        return true;
    }

    this->eaMinMax = std::pair<gtirb::EA, gtirb::EA>(gtirb::EA{}, gtirb::EA{});
    return false;
}

std::pair<gtirb::EA, gtirb::EA> ImageByteMap::getEAMinMax() const
{
    return this->eaMinMax;
}

void ImageByteMap::setRebaseDelta(int64_t x)
{
    this->rebaseDelta = x;
}

int64_t ImageByteMap::getRebaseDelta() const
{
    return this->rebaseDelta;
}

void ImageByteMap::setLFCM(uint8_t x)
{
    this->lfcm = x;
}

uint8_t ImageByteMap::getLFCM() const
{
    return this->lfcm;
}

void ImageByteMap::setIsRelocated()
{
    this->isRelocated = true;
}

bool ImageByteMap::getIsRelocated() const
{
    return this->isRelocated;
}

void ImageByteMap::setGlobalOffsetTableAddress(EA x)
{
    this->globalOffsetTableAddress = x;
}

EA ImageByteMap::getGlobalOffsetTableAddress() const
{
    return this->globalOffsetTableAddress;
}

void ImageByteMap::setContentSource(ImageByteMap::ContentSource x)
{
    this->contentSource = x;
}

ImageByteMap::ContentSource ImageByteMap::getContentSource() const
{
    return this->contentSource;
}

bool ImageByteMap::getDataEmpty() const
{
    return this->byteMap.empty();
}

size_t ImageByteMap::getDataSize() const
{
    return this->byteMap.size();
}

void ImageByteMap::setData(EA ea, uint8_t x)
{
    if(ea >= this->eaMinMax.first && (ea + EA{sizeof(x)} - EA{1}) <= this->eaMinMax.second)
    {
        this->byteMap.setData(ea, x);
    }
    else
    {
        throw std::out_of_range("Attempt to set data at an EA out of range of the min and max EA.");
    }
}

void ImageByteMap::setData(EA ea, uint16_t x)
{
    if(ea >= this->eaMinMax.first && (ea + EA{sizeof(x)} - EA{1}) <= this->eaMinMax.second)
    {
        this->byteMap.setData(ea, x);
    }
    else
    {
        throw std::out_of_range("Attempt to set data at an EA out of range of the min and max EA.");
    }
}

void ImageByteMap::setData(EA ea, uint32_t x)
{
    if(ea >= this->eaMinMax.first && (ea + EA{sizeof(x)} - EA{1}) <= this->eaMinMax.second)
    {
        this->byteMap.setData(ea, x);
    }
    else
    {
        throw std::out_of_range("Attempt to set data at an EA out of range of the min and max EA.");
    }
}

void ImageByteMap::setData(EA ea, uint64_t x)
{
    if(ea >= this->eaMinMax.first && (ea + EA{sizeof(x)} - EA{1}) <= this->eaMinMax.second)
    {
        this->byteMap.setData(ea, x);
    }
    else
    {
        throw std::out_of_range("Attempt to set data at an EA out of range of the min and max EA.");
    }
}

void ImageByteMap::setData(EA ea, uint8_t* const x, size_t bytes)
{
    if(ea >= this->eaMinMax.first && (ea + EA{bytes} - EA{1}) <= this->eaMinMax.second)
    {
        this->byteMap.setData(ea, x, bytes);
    }
    else
    {
        throw std::out_of_range("Attempt to set data at an EA out of range of the min and max EA.");
    }
}

uint8_t ImageByteMap::getData8(EA x) const
{
    if(x >= this->eaMinMax.first && (x + EA{sizeof(uint8_t)} - EA{1}) <= this->eaMinMax.second)
    {
        return this->byteMap.getData8(x);
    }

    throw std::out_of_range("Attempt to get data at an EA out of range of the min and max EA.");
}

uint16_t ImageByteMap::getData16(EA x) const
{
    if(x >= this->eaMinMax.first && (x + EA{sizeof(uint16_t)} - EA{1}) <= this->eaMinMax.second)
    {
        return this->byteMap.getData16(x);
    }

    throw std::out_of_range("Attempt to get data at an EA out of range of the min and max EA.");
}

uint32_t ImageByteMap::getData32(EA x) const
{
    if(x >= this->eaMinMax.first && (x + EA{sizeof(uint32_t)} - EA{1}) <= this->eaMinMax.second)
    {
        return this->byteMap.getData32(x);
    }

    throw std::out_of_range("Attempt to get data at an EA out of range of the min and max EA.");
}

uint64_t ImageByteMap::getData64(EA x) const
{
    if(x >= this->eaMinMax.first && (x + EA{sizeof(uint64_t)} - EA{1}) <= this->eaMinMax.second)
    {
        return this->byteMap.getData64(x);
    }

    throw std::out_of_range("Attempt to get data at an EA out of range of the min and max EA.");
}

std::vector<uint8_t> ImageByteMap::getData(EA x, size_t bytes) const
{
    if(x >= this->eaMinMax.first && (x + EA{bytes} - EA{1}) <= this->eaMinMax.second)
    {
        return this->byteMap.getData(x, bytes);
    }

    throw std::out_of_range("Attempt to get data at an EA out of range of the min and max EA.");
}

std::vector<uint8_t> ImageByteMap::getDataUntil(EA ea, uint8_t sentinel, size_t bytes) const
{
    if(ea >= this->eaMinMax.first && (ea) <= this->eaMinMax.second)
    {
        return this->byteMap.getDataUntil(ea, sentinel, bytes);
    }

    throw std::out_of_range("Attempt to get data at an EA out of range of the min and max EA.");
}
