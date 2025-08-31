// IGED Analytics Dashboard - Real-time Analytics and Metrics
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDAnalyticsDashboard.generated.h"

// Analytics data structure
USTRUCT(BlueprintType)
struct FAnalyticsData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FString MetricName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    float Value;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FString Unit;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FString Category;

    FAnalyticsData()
        : Value(0.0f)
    {}
};

// Dashboard widget structure
USTRUCT(BlueprintType)
struct FDashboardWidget
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FString WidgetName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FString WidgetType;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    TArray<FAnalyticsData> Data;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    bool bIsVisible;

    FDashboardWidget()
        : bIsVisible(true)
    {}
};

// Performance trend structure
USTRUCT(BlueprintType)
struct FPerformanceTrend
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    FString TrendName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    TArray<float> Values;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    float AverageValue;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    float MinValue;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    float MaxValue;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Analytics")
    float TrendDirection; // -1 for decreasing, 0 for stable, 1 for increasing

    FPerformanceTrend()
        : AverageValue(0.0f)
        , MinValue(0.0f)
        , MaxValue(0.0f)
        , TrendDirection(0.0f)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnalyticsUpdated, const FAnalyticsData&, NewData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDashboardRefreshed, const TArray<FDashboardWidget>&, Widgets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrendDetected, const FPerformanceTrend&, Trend);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDAnalyticsDashboard : public UObject
{
    GENERATED_BODY()

public:
    UIGEDAnalyticsDashboard();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void Initialize();

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void Shutdown();

    // Data collection
    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void AddAnalyticsData(const FAnalyticsData& Data);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void UpdateMetric(const FString& MetricName, float Value, const FString& Unit = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void StartRealTimeMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void StopRealTimeMonitoring();

    // Dashboard management
    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void CreateDashboardWidget(const FString& WidgetName, const FString& WidgetType);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void UpdateDashboardWidget(const FString& WidgetName, const TArray<FAnalyticsData>& Data);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void RefreshDashboard();

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    TArray<FDashboardWidget> GetDashboardWidgets();

    // Analytics and trends
    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    FPerformanceTrend CalculateTrend(const FString& MetricName, int32 DataPoints = 10);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    TArray<FAnalyticsData> GetAnalyticsHistory(const FString& MetricName);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    float GetAverageValue(const FString& MetricName, int32 DataPoints = 10);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    float GetMinValue(const FString& MetricName, int32 DataPoints = 10);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    float GetMaxValue(const FString& MetricName, int32 DataPoints = 10);

    // Advanced analytics
    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void GeneratePerformanceInsights();

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    FString GenerateAnalyticsReport();

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void ExportAnalyticsData(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
    void ImportAnalyticsData(const FString& FilePath);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Analytics|Events")
    FOnAnalyticsUpdated OnAnalyticsUpdated;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Analytics|Events")
    FOnDashboardRefreshed OnDashboardRefreshed;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Analytics|Events")
    FOnTrendDetected OnTrendDetected;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    TArray<FAnalyticsData> AnalyticsHistory;

    UPROPERTY()
    TArray<FDashboardWidget> DashboardWidgets;

    UPROPERTY()
    TMap<FString, TArray<FAnalyticsData>> MetricHistory;

private:
    void UpdateRealTimeMetrics();
    void ProcessAnalyticsData();
    void DetectTrends();
    void CleanupOldData();
    
    // Analytics processing
    float CalculateTrendDirection(const TArray<float>& Values);
    void GenerateInsights();
    void UpdateWidgetData();
    
    // Timer handles
    FTimerHandle RealTimeMonitoringTimer;
    FTimerHandle AnalyticsProcessingTimer;
};
