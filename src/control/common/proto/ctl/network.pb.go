// Code generated by protoc-gen-go. DO NOT EDIT.
// source: network.proto

package ctl

import (
	fmt "fmt"
	proto "github.com/golang/protobuf/proto"
	math "math"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

type NetworkScanReq struct {
	Provider             string   `protobuf:"bytes,1,opt,name=provider,proto3" json:"provider,omitempty"`
	Excludeinterfaces    string   `protobuf:"bytes,2,opt,name=excludeinterfaces,proto3" json:"excludeinterfaces,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *NetworkScanReq) Reset()         { *m = NetworkScanReq{} }
func (m *NetworkScanReq) String() string { return proto.CompactTextString(m) }
func (*NetworkScanReq) ProtoMessage()    {}
func (*NetworkScanReq) Descriptor() ([]byte, []int) {
	return fileDescriptor_8571034d60397816, []int{0}
}

func (m *NetworkScanReq) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_NetworkScanReq.Unmarshal(m, b)
}
func (m *NetworkScanReq) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_NetworkScanReq.Marshal(b, m, deterministic)
}
func (m *NetworkScanReq) XXX_Merge(src proto.Message) {
	xxx_messageInfo_NetworkScanReq.Merge(m, src)
}
func (m *NetworkScanReq) XXX_Size() int {
	return xxx_messageInfo_NetworkScanReq.Size(m)
}
func (m *NetworkScanReq) XXX_DiscardUnknown() {
	xxx_messageInfo_NetworkScanReq.DiscardUnknown(m)
}

var xxx_messageInfo_NetworkScanReq proto.InternalMessageInfo

func (m *NetworkScanReq) GetProvider() string {
	if m != nil {
		return m.Provider
	}
	return ""
}

func (m *NetworkScanReq) GetExcludeinterfaces() string {
	if m != nil {
		return m.Excludeinterfaces
	}
	return ""
}

type NetworkScanResp struct {
	Interfaces           []*FabricInterface `protobuf:"bytes,1,rep,name=interfaces,proto3" json:"interfaces,omitempty"`
	Numacount            int32              `protobuf:"varint,2,opt,name=numacount,proto3" json:"numacount,omitempty"`
	Corespernuma         int32              `protobuf:"varint,3,opt,name=corespernuma,proto3" json:"corespernuma,omitempty"`
	XXX_NoUnkeyedLiteral struct{}           `json:"-"`
	XXX_unrecognized     []byte             `json:"-"`
	XXX_sizecache        int32              `json:"-"`
}

func (m *NetworkScanResp) Reset()         { *m = NetworkScanResp{} }
func (m *NetworkScanResp) String() string { return proto.CompactTextString(m) }
func (*NetworkScanResp) ProtoMessage()    {}
func (*NetworkScanResp) Descriptor() ([]byte, []int) {
	return fileDescriptor_8571034d60397816, []int{1}
}

func (m *NetworkScanResp) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_NetworkScanResp.Unmarshal(m, b)
}
func (m *NetworkScanResp) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_NetworkScanResp.Marshal(b, m, deterministic)
}
func (m *NetworkScanResp) XXX_Merge(src proto.Message) {
	xxx_messageInfo_NetworkScanResp.Merge(m, src)
}
func (m *NetworkScanResp) XXX_Size() int {
	return xxx_messageInfo_NetworkScanResp.Size(m)
}
func (m *NetworkScanResp) XXX_DiscardUnknown() {
	xxx_messageInfo_NetworkScanResp.DiscardUnknown(m)
}

var xxx_messageInfo_NetworkScanResp proto.InternalMessageInfo

func (m *NetworkScanResp) GetInterfaces() []*FabricInterface {
	if m != nil {
		return m.Interfaces
	}
	return nil
}

func (m *NetworkScanResp) GetNumacount() int32 {
	if m != nil {
		return m.Numacount
	}
	return 0
}

func (m *NetworkScanResp) GetCorespernuma() int32 {
	if m != nil {
		return m.Corespernuma
	}
	return 0
}

type FabricInterface struct {
	Provider             string   `protobuf:"bytes,1,opt,name=provider,proto3" json:"provider,omitempty"`
	Device               string   `protobuf:"bytes,2,opt,name=device,proto3" json:"device,omitempty"`
	Numanode             uint32   `protobuf:"varint,3,opt,name=numanode,proto3" json:"numanode,omitempty"`
	Priority             uint32   `protobuf:"varint,4,opt,name=priority,proto3" json:"priority,omitempty"`
	Netdevclass          uint32   `protobuf:"varint,5,opt,name=netdevclass,proto3" json:"netdevclass,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *FabricInterface) Reset()         { *m = FabricInterface{} }
func (m *FabricInterface) String() string { return proto.CompactTextString(m) }
func (*FabricInterface) ProtoMessage()    {}
func (*FabricInterface) Descriptor() ([]byte, []int) {
	return fileDescriptor_8571034d60397816, []int{2}
}

func (m *FabricInterface) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_FabricInterface.Unmarshal(m, b)
}
func (m *FabricInterface) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_FabricInterface.Marshal(b, m, deterministic)
}
func (m *FabricInterface) XXX_Merge(src proto.Message) {
	xxx_messageInfo_FabricInterface.Merge(m, src)
}
func (m *FabricInterface) XXX_Size() int {
	return xxx_messageInfo_FabricInterface.Size(m)
}
func (m *FabricInterface) XXX_DiscardUnknown() {
	xxx_messageInfo_FabricInterface.DiscardUnknown(m)
}

var xxx_messageInfo_FabricInterface proto.InternalMessageInfo

func (m *FabricInterface) GetProvider() string {
	if m != nil {
		return m.Provider
	}
	return ""
}

func (m *FabricInterface) GetDevice() string {
	if m != nil {
		return m.Device
	}
	return ""
}

func (m *FabricInterface) GetNumanode() uint32 {
	if m != nil {
		return m.Numanode
	}
	return 0
}

func (m *FabricInterface) GetPriority() uint32 {
	if m != nil {
		return m.Priority
	}
	return 0
}

func (m *FabricInterface) GetNetdevclass() uint32 {
	if m != nil {
		return m.Netdevclass
	}
	return 0
}

func init() {
	proto.RegisterType((*NetworkScanReq)(nil), "ctl.NetworkScanReq")
	proto.RegisterType((*NetworkScanResp)(nil), "ctl.NetworkScanResp")
	proto.RegisterType((*FabricInterface)(nil), "ctl.FabricInterface")
}

func init() {
	proto.RegisterFile("network.proto", fileDescriptor_8571034d60397816)
}

var fileDescriptor_8571034d60397816 = []byte{
	// 254 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x91, 0xb1, 0x4e, 0xc3, 0x30,
	0x18, 0x84, 0x65, 0x42, 0x2b, 0xfa, 0x97, 0x52, 0x61, 0x21, 0x64, 0x21, 0x86, 0x28, 0x53, 0x07,
	0x94, 0x01, 0x78, 0x06, 0x24, 0x16, 0x06, 0xb3, 0xb1, 0xb9, 0xbf, 0x7f, 0x24, 0x8b, 0x60, 0x07,
	0xdb, 0x09, 0xf0, 0x08, 0x3c, 0x05, 0xaf, 0x8a, 0xe2, 0xb4, 0x25, 0x01, 0xa9, 0xe3, 0x7d, 0x77,
	0xbe, 0xd3, 0x2f, 0xc3, 0xc2, 0x52, 0x7c, 0x77, 0xfe, 0xa5, 0xac, 0xbd, 0x8b, 0x8e, 0x67, 0x18,
	0xab, 0xe2, 0x09, 0x4e, 0x1e, 0x7a, 0xfa, 0x88, 0xca, 0x4a, 0x7a, 0xe3, 0x17, 0x70, 0x54, 0x7b,
	0xd7, 0x1a, 0x4d, 0x5e, 0xb0, 0x9c, 0xad, 0x66, 0x72, 0xa7, 0xf9, 0x15, 0x9c, 0xd2, 0x07, 0x56,
	0x8d, 0x26, 0x63, 0x23, 0xf9, 0x67, 0x85, 0x14, 0xc4, 0x41, 0x0a, 0xfd, 0x37, 0x8a, 0x2f, 0x06,
	0xcb, 0x51, 0x79, 0xa8, 0xf9, 0x2d, 0xc0, 0xe0, 0x29, 0xcb, 0xb3, 0xd5, 0xfc, 0xfa, 0xac, 0xc4,
	0x58, 0x95, 0x77, 0x6a, 0xed, 0x0d, 0xde, 0x6f, 0x4d, 0x39, 0xc8, 0xf1, 0x4b, 0x98, 0xd9, 0xe6,
	0x55, 0xa1, 0x6b, 0x6c, 0x4c, 0x7b, 0x13, 0xf9, 0x0b, 0x78, 0x01, 0xc7, 0xe8, 0x3c, 0x85, 0x9a,
	0x7c, 0x07, 0x45, 0x96, 0x02, 0x23, 0x56, 0x7c, 0x33, 0x58, 0xfe, 0x59, 0xd8, 0x7b, 0xe9, 0x39,
	0x4c, 0x35, 0xb5, 0x06, 0x69, 0x73, 0xde, 0x46, 0x75, 0x6f, 0xba, 0x3e, 0xeb, 0x34, 0xa5, 0x9d,
	0x85, 0xdc, 0xe9, 0xbe, 0xcf, 0x38, 0x6f, 0xe2, 0xa7, 0x38, 0xec, 0xbd, 0xad, 0xe6, 0x39, 0xcc,
	0x2d, 0x45, 0x4d, 0x2d, 0x56, 0x2a, 0x04, 0x31, 0x49, 0xf6, 0x10, 0xad, 0xa7, 0xe9, 0x57, 0x6e,
	0x7e, 0x02, 0x00, 0x00, 0xff, 0xff, 0xf6, 0x36, 0x88, 0x1e, 0xa6, 0x01, 0x00, 0x00,
}
