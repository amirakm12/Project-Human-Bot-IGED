// IGEDQuantumComputing.h - QUANTUM COMPUTING SIMULATION
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDQuantumComputing.generated.h"

USTRUCT(BlueprintType)
struct FQuantumComplex
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float Real;

    UPROPERTY(BlueprintReadWrite)
    float Imaginary;

    FQuantumComplex()
    {
        Real = 0.0f;
        Imaginary = 0.0f;
    }

    FQuantumComplex(float R, float I)
    {
        Real = R;
        Imaginary = I;
    }

    FQuantumComplex operator+(const FQuantumComplex& Other) const
    {
        return FQuantumComplex(Real + Other.Real, Imaginary + Other.Imaginary);
    }

    FQuantumComplex operator-(const FQuantumComplex& Other) const
    {
        return FQuantumComplex(Real - Other.Real, Imaginary - Other.Imaginary);
    }

    FQuantumComplex operator*(const FQuantumComplex& Other) const
    {
        return FQuantumComplex(
            Real * Other.Real - Imaginary * Other.Imaginary,
            Real * Other.Imaginary + Imaginary * Other.Real
        );
    }

    FQuantumComplex operator*(float Scalar) const
    {
        return FQuantumComplex(Real * Scalar, Imaginary * Scalar);
    }

    float GetMagnitude() const
    {
        return FMath::Sqrt(Real * Real + Imaginary * Imaginary);
    }
};

UENUM(BlueprintType)
enum class EQuantumGateType : uint8
{
    Hadamard,
    PauliX,
    PauliY,
    PauliZ,
    CNOT,
    Toffoli,
    Rotation,
    Phase,
    SWAP
};

UENUM(BlueprintType)
enum class ERotationAxis : uint8
{
    X,
    Y,
    Z
};

USTRUCT(BlueprintType)
struct FQuantumGate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    EQuantumGateType Type;

    UPROPERTY(BlueprintReadWrite)
    int32 TargetQubit;

    UPROPERTY(BlueprintReadWrite)
    int32 ControlQubit;

    UPROPERTY(BlueprintReadWrite)
    int32 ControlQubit2;

    UPROPERTY(BlueprintReadWrite)
    float RotationAngle;

    FQuantumGate()
    {
        Type = EQuantumGateType::Hadamard;
        TargetQubit = 0;
        ControlQubit = -1;
        ControlQubit2 = -1;
        RotationAngle = 0.0f;
    }
};

UCLASS(BlueprintType)
class IGED_API UIGEDQuantumComputing : public UObject
{
    GENERATED_BODY()

public:
    UIGEDQuantumComputing();

    // Circuit initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum")
    void InitializeQuantumCircuit(int32 Qubits);

    // Single-qubit gates
    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyHadamardGate(int32 QubitIndex);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyPauliX(int32 QubitIndex);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyPauliY(int32 QubitIndex);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyPauliZ(int32 QubitIndex);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyRotationGate(int32 QubitIndex, float Theta, ERotationAxis Axis);

    // Multi-qubit gates
    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyCNOT(int32 ControlQubit, int32 TargetQubit);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void ApplyToffoli(int32 Control1, int32 Control2, int32 Target);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Gates")
    void SwapQubits(int32 Qubit1, int32 Qubit2);

    // Quantum algorithms
    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Algorithms")
    void ApplyQuantumFourierTransform();

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Algorithms")
    void RunGroverSearch(int32 MarkedItem);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Algorithms")
    void CreateEntanglement(int32 Qubit1, int32 Qubit2);

    // Measurement
    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Measurement")
    TArray<int32> MeasureQubits();

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Measurement")
    void RunQuantumCircuit(int32 Shots = 1024);

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Measurement")
    TMap<FString, float> GetMeasurementStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Quantum|Measurement")
    float CalculateQuantumEntropy();

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|Quantum|Config")
    int32 NumQubits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|Quantum|Config")
    int32 NumShots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|Quantum|Config")
    float NoiseLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|Quantum|Config")
    bool bUseErrorCorrection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|Quantum|Config")
    bool bUseQuantumEntanglement;

protected:
    void ApplyControlledPhase(int32 ControlQubit, int32 TargetQubit, float Phase);
    void ApplyDiffusionOperator();

private:
    TArray<FQuantumComplex> QuantumState;
    TArray<FQuantumGate> QuantumGates;
    TArray<TArray<int32>> MeasurementResults;
};