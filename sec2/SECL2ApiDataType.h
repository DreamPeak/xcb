/**
 * 版权所有(C)2014-2018, 大连飞创信息技术有限公司
 * 文件名称：SECL2ApiDataType.h
 * 文件说明：定义接口所需的数据类型的头文件
 * 当前版本：1.0.2
 * 作者：Datafeed项目组
 * 发布日期：2015年04月29日
 */             
#if !defined(AFX_HYQUOTE_H__INCLUDED_)
#define AFX_HYQUOTE_H__INCLUDED_


#ifdef WIN32
	typedef unsigned __int64    uint64;	
	typedef __int64             int64;
#else
	typedef unsigned long       uint64;	
	typedef long                int64;
#endif
	typedef unsigned int        uint32;
	typedef unsigned short      uint16;
	typedef unsigned char       uint8;
	typedef int                 int32;
	typedef short               int16;
	typedef char                int8;

#pragma pack(1)

struct SHFAST30_Prices_t{
    // 操作符
    // PriceLevelOperator  PriceOps;
    // 价格
    float  Price;
    // 差量镜像合并后实际的总委托量
    uint64  OrderQty;
    // 差量镜像合并后实际的总委托笔数
    uint32  NumOrders;
};

// 市场行情
struct SHFAST30_UA3202_t
{
    // UA3202
    int8 MessageType[8];
    // 最新订单时间(秒)
    uint32 DataTimeStamp;	
    // 1-重复数据 2-未授权
    //uint32 DataStatus;	
    // 证券代码
    int8 SecurityID[9];	
    // 快照类型 1-全量 2-更新
    //uint32 imageStatus;
	
    // 昨收盘价
    float PreClosePx;	
    // 开盘价
    float OpenPx;
    // 最高价
    float HighPx;
    // 最低价
    float LowPx;
    // 现价
    float LastPx;	
    // 今日收盘价
    float ClosePx;
	
    // 当前品种交易状态
    int8 InstrumentStatus[8];
	
    // 成交笔数
    uint32 NumTrades;
    // 成交总量
    uint64 TotalVolumeTrade;
    // 成交总金额
    double TotalValueTrade;
	
    // 委托买入总量
    uint64 TotalBidQty;
    // 加权平均委买价格
    float WeightedAvgBidPx;
    // 债券加权平均委买价格
    float AltWeightedAvgBidPx;
    // 委托卖出总量
    uint64 TotalOfferQty;
    // 加权平均委卖价格
    float WeightedAvgOfferPx;
    // 债券加权平均委卖价格
    float AltWeightedAvgOfferPx;
	
    // ETF净值估值
    uint32 IOPV;
    // ETF申购笔数
    uint32 ETFBuyNumber;
    // ETF申购数量
    uint64 ETFBuyAmount;
    // ETF申购金额
    double ETFBuyMoney;
    // ETF赎回笔数
    uint32 ETFSellNumber;
    // ETF赎回数量
    uint64 ETFSellAmount;
    // ETF赎回金额
    double ETFSellMoney;
	
    // 债券到期收益率
    float YieldToMaturity;
    // 权证执行总数量
    uint64 TotalWarrantExecQty;
    // 权证跌停价格
    double WarLowerPx;
    // 权证涨停价格
    double WarUpperPx;
	
    // 买入撤单笔数
    uint32 WithdrawBuyNumber;
    // 买入撤单数量
    uint64 WithdrawBuyAmount;
    // 买入撤单金额
    double WithdrawBuyMoney;
    // 卖出撤单笔数
    uint32 WithdrawSellNumber;
    // 卖出撤单数量
    uint64 WithdrawSellAmount;
    // 卖出撤单金额
    double WithdrawSellMoney;
    // 买入总笔数
    uint32 TotalBidNumber;
    // 卖出总笔数
    uint32 TotalOfferNumber;
    // 买入委托成交最大等待时间
    uint32 BidTradeMaxDuration;
    // 卖出委托成交最大等待时间
    uint32 OfferTradeMaxDuration;
    // 买方委托价位数
    uint32 NumBidOrders;
    // 卖方委托价位数
    uint32 NumOfferOrders;

    // 委买价格个数
    uint32 NoBidLevel;
    // 委买价格数组
    SHFAST30_Prices_t arrBidPriceLevels[10];

    // 委卖价格个数
    uint32 NoOfferLevel;
    // 委卖价格数组
    SHFAST30_Prices_t arrOfferPriceLevels[10];
};

// 逐笔成交
struct SHFAST30_UA3201_t
{
    // UA3201
    int8 MessageType[8];
    // 1-重复数据 2-未授权
    //uint32 DataStatus;
    // 成交序号
    uint32 TradeIndex;
    // 成交通道
    uint32 TradeChannel;
    // 证券代码
    int8 SecurityID[9];
    // 成交时间(百分之一秒)
    uint32 TradeTime;
    // 成交价格
    float TradePrice;
    // 成交数量
    uint64 TradeQty;
    // 成交金额
    double TradeMoney;
    // 买方订单号
    uint64 TradeBuyNo;
    // 卖方订单号
    uint64 TradeSellNo;
    // 内外盘标志: B-外盘买 S-内盘卖 N-未知
    int8 TradeBSFlag[8];
};

// 指数行情
struct SHFAST30_UA3113_t
{
    // UA3113
    int8 MessageType [8];
    // 行情时间(秒)
    uint32 DataTimestamp;
    // 1-重复数据 2-未授权
    //uint32 DataStatus;
    // 证券代码
    int8 SecurityID[9];
    // 前收盘指数
    double PreCloseIndex;
    // 今收盘指数
    double OpenIndex;
    // 参与计算相应指数的成交金额
    double Turnover;
    // 最高指数
    double HighIndex;
    // 最低指数
    double LowIndex;
    // 最新指数
    double LastIndex;
    // 成交时间
    uint32 TradeTime;
    // 参与计算相应指数的交易数量
    uint64 TotalVolumeTraded;
    // 今日收盘指数
    double CloseIndex;
};

// 市场总览
struct SHFAST30_UA3115_t
{
    // UA3115
    int8 MessageType [8];
    // 行情时间(秒)
    uint32 DataTimestamp;
    // 1-重复数据 2-未授权
    //uint32 DataStatus;
    // 证券代码
    int8 SecurityID[9];
    // 市场时间(百分之一秒)
    uint32 OrigTime;
    // 市场日期
    uint32 OrigDate;
};

#pragma pack()


#pragma pack(4)
////////////////////////////////////////////////
///DFITCUserLoginField：登录信息
////////////////////////////////////////////////
struct SECUserLoginField
{
    int8		accountID[30];					//用户名
    int8		passwd[30];						//密码
};

////////////////////////////////////////////////
///DFITCPasswdChangeField：修改密码信息
////////////////////////////////////////////////
struct SECPasswdChangeField
{
    int8		AccountID[30];					//用户名
    int8		OldPassword[30];				//旧密码
    int8		NewPassword[30];				//新密码
};

////////////////////////////////////////////////
///DFITCUserLogoutField：登出信息
////////////////////////////////////////////////
struct SECUserLogoutField
{
    int8		AccountID[30];					//用户名
};

////////////////////////////////////////////////
///ErrorRtnField：登录，登出，修改密码返回信息
////////////////////////////////////////////////
struct ErrorRtnField
{
    int8		ErrorID;						//0为成功 非0为失败
    int8		ErrorMsg[128];					//失败时，返回错误信息					
};

#pragma pack()

#endif // !defined(AFX_HYQUOTE_H__INCLUDED_)
