import * as React from "react"
import { Slot } from "@radix-ui/react-slot"
import { cva, type VariantProps } from "class-variance-authority"
import { cn } from "../../lib/utils"

const buttonVariants = cva(
  "iged-button inline-flex items-center justify-center whitespace-nowrap rounded-iged-lg text-iged-sm font-semibold ring-offset-background transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50",
  {
    variants: {
      variant: {
        // Sovereign AI Primary - Matrix green gradient
        sovereign: "bg-iged-gradient text-iged-text-primary shadow-iged-glow hover:shadow-iged-glow-blue hover:-translate-y-0.5 transition-all duration-300",
        
        // Neural Network Blue  
        neural: "bg-iged-accent-secondary text-iged-text-primary shadow-iged-glow-blue hover:bg-opacity-90 hover:shadow-iged-xl",
        
        // Matrix Terminal Green
        matrix: "bg-iged-accent-primary text-iged-bg-primary shadow-iged-glow hover:bg-opacity-90 font-iged-mono text-glow-green",
        
        // Danger/Alert Red
        danger: "bg-iged-danger text-iged-text-primary hover:bg-red-600 shadow-lg",
        
        // Ghost - Transparent with border
        ghost: "sovereign-border bg-transparent text-iged-accent-primary hover:bg-iged-accent-primary hover:text-iged-bg-primary",
        
        // Subtle dark variant
        dark: "bg-iged-bg-tertiary text-iged-text-primary hover:bg-iged-bg-secondary border border-iged-bg-tertiary",
        
        // Glass effect variant
        glass: "glass-effect text-iged-text-primary hover:bg-opacity-90 backdrop-blur-iged",
        
        // Link style
        link: "text-iged-accent-primary underline-offset-4 hover:underline text-glow-blue",
      },
      size: {
        default: "h-10 px-iged-lg py-iged-sm",
        sm: "h-9 rounded-iged-md px-iged-md text-iged-xs",
        lg: "h-11 rounded-iged-lg px-iged-xl text-iged-lg",
        xl: "h-14 rounded-iged-xl px-iged-2xl text-iged-xl font-bold",
        icon: "h-10 w-10",
      },
      glow: {
        none: "",
        subtle: "shadow-iged-sm",
        normal: "shadow-iged-glow",
        intense: "shadow-iged-xl animate-sovereign-glow",
      },
      animation: {
        none: "",
        pulse: "animate-neural-pulse",
        glow: "animate-sovereign-glow",
        fadeIn: "animate-fade-in",
      }
    },
    defaultVariants: {
      variant: "sovereign",
      size: "default",
      glow: "normal",
      animation: "none",
    },
  }
)

export interface ButtonProps
  extends React.ButtonHTMLAttributes<HTMLButtonElement>,
    VariantProps<typeof buttonVariants> {
  asChild?: boolean
  icon?: React.ReactNode
  iconPosition?: "left" | "right"
  loading?: boolean
  loadingText?: string
}

const Button = React.forwardRef<HTMLButtonElement, ButtonProps>(
  ({ 
    className, 
    variant, 
    size, 
    glow, 
    animation,
    asChild = false, 
    icon,
    iconPosition = "left",
    loading = false,
    loadingText = "Loading...",
    children,
    disabled,
    ...props 
  }, ref) => {
    const Comp = asChild ? Slot : "button"
    
    const isDisabled = disabled || loading

    const renderContent = () => {
      if (loading) {
        return (
          <>
            <div className="animate-spin mr-2 h-4 w-4 border-2 border-current border-t-transparent rounded-full" />
            {loadingText}
          </>
        )
      }

      if (icon && iconPosition === "left") {
        return (
          <>
            <span className="mr-2 flex-shrink-0">{icon}</span>
            {children}
          </>
        )
      }

      if (icon && iconPosition === "right") {
        return (
          <>
            {children}
            <span className="ml-2 flex-shrink-0">{icon}</span>
          </>
        )
      }

      return children
    }

    return (
      <Comp
        className={cn(
          buttonVariants({ variant, size, glow, animation, className }),
          isDisabled && "opacity-50 cursor-not-allowed",
          // Add shimmer effect on hover
          "before:absolute before:inset-0 before:bg-gradient-to-r before:from-transparent before:via-white/10 before:to-transparent before:translate-x-[-100%] hover:before:translate-x-[100%] before:transition-transform before:duration-500"
        )}
        ref={ref}
        disabled={isDisabled}
        {...props}
      >
        {renderContent()}
      </Comp>
    )
  }
)
Button.displayName = "Button"

// Specialized button components for common IGED use cases
export const SovereignButton = React.forwardRef<HTMLButtonElement, Omit<ButtonProps, 'variant'>>(
  (props, ref) => <Button variant="sovereign" glow="intense" animation="glow" {...props} ref={ref} />
)
SovereignButton.displayName = "SovereignButton"

export const MatrixButton = React.forwardRef<HTMLButtonElement, Omit<ButtonProps, 'variant'>>(
  (props, ref) => <Button variant="matrix" glow="normal" {...props} ref={ref} />
)
MatrixButton.displayName = "MatrixButton"

export const NeuralButton = React.forwardRef<HTMLButtonElement, Omit<ButtonProps, 'variant'>>(
  (props, ref) => <Button variant="neural" animation="pulse" {...props} ref={ref} />
)
NeuralButton.displayName = "NeuralButton"

export const GhostButton = React.forwardRef<HTMLButtonElement, Omit<ButtonProps, 'variant'>>(
  (props, ref) => <Button variant="ghost" glow="subtle" {...props} ref={ref} />
)
GhostButton.displayName = "GhostButton"

export { Button, buttonVariants }