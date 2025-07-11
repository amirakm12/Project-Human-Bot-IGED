import * as React from "react"
import { cva, type VariantProps } from "class-variance-authority"
import { cn } from "../../lib/utils"

const cardVariants = cva(
  "iged-card rounded-iged-xl border border-white/10 text-iged-text-primary shadow-iged-lg transition-all duration-300",
  {
    variants: {
      variant: {
        // Standard dark card
        default: "bg-iged-bg-secondary hover:border-white/15 hover:shadow-iged-xl",
        
        // Neural network themed with grid pattern
        neural: "bg-iged-bg-secondary neural-grid-bg hover:border-iged-accent-secondary/30 hover:shadow-iged-glow-blue",
        
        // Glass effect card
        glass: "glass-effect backdrop-blur-iged hover:bg-opacity-90",
        
        // Matrix terminal style
        matrix: "bg-iged-bg-input border-iged-accent-primary/30 hover:border-iged-accent-primary/50 hover:shadow-iged-glow",
        
        // Elevated with stronger shadow
        elevated: "bg-iged-bg-secondary shadow-iged-xl hover:shadow-2xl transform hover:-translate-y-1",
        
        // Sovereign style with gradient border
        sovereign: "bg-iged-bg-secondary sovereign-border hover:animate-sovereign-glow"
      },
      size: {
        sm: "p-iged-md",
        default: "p-iged-xl", 
        lg: "p-iged-2xl",
        xl: "p-iged-3xl"
      },
      glow: {
        none: "",
        subtle: "shadow-iged-sm",
        normal: "shadow-iged-glow",
        intense: "shadow-iged-xl animate-neural-pulse"
      }
    },
    defaultVariants: {
      variant: "default",
      size: "default",
      glow: "none"
    },
  }
)

export interface CardProps
  extends React.HTMLAttributes<HTMLDivElement>,
    VariantProps<typeof cardVariants> {
  animated?: boolean
}

const Card = React.forwardRef<HTMLDivElement, CardProps>(
  ({ className, variant, size, glow, animated = false, ...props }, ref) => (
    <div
      ref={ref}
      className={cn(
        cardVariants({ variant, size, glow }),
        animated && "animate-fade-in",
        className
      )}
      {...props}
    />
  )
)
Card.displayName = "Card"

const CardHeader = React.forwardRef<
  HTMLDivElement,
  React.HTMLAttributes<HTMLDivElement>
>(({ className, ...props }, ref) => (
  <div
    ref={ref}
    className={cn(
      "flex flex-col space-y-1.5 pb-iged-md border-b-2 border-iged-accent-secondary/30 mb-iged-xl",
      className
    )}
    {...props}
  />
))
CardHeader.displayName = "CardHeader"

const CardTitle = React.forwardRef<
  HTMLParagraphElement,
  React.HTMLAttributes<HTMLHeadingElement>
>(({ className, children, ...props }, ref) => (
  <h3
    ref={ref}
    className={cn(
      "text-iged-2xl font-semibold leading-none tracking-tight text-iged-text-primary",
      className
    )}
    {...props}
  >
    {children}
  </h3>
))
CardTitle.displayName = "CardTitle"

const CardDescription = React.forwardRef<
  HTMLParagraphElement,
  React.HTMLAttributes<HTMLParagraphElement>
>(({ className, ...props }, ref) => (
  <p
    ref={ref}
    className={cn("text-iged-sm text-iged-text-secondary", className)}
    {...props}
  />
))
CardDescription.displayName = "CardDescription"

const CardContent = React.forwardRef<
  HTMLDivElement,
  React.HTMLAttributes<HTMLDivElement>
>(({ className, ...props }, ref) => (
  <div ref={ref} className={cn("", className)} {...props} />
))
CardContent.displayName = "CardContent"

const CardFooter = React.forwardRef<
  HTMLDivElement,
  React.HTMLAttributes<HTMLDivElement>
>(({ className, ...props }, ref) => (
  <div
    ref={ref}
    className={cn(
      "flex items-center pt-iged-md border-t border-white/10 mt-iged-xl",
      className
    )}
    {...props}
  />
))
CardFooter.displayName = "CardFooter"

// Specialized card variants for IGED use cases
interface StatusCardProps extends Omit<CardProps, 'variant'> {
  status: 'online' | 'offline' | 'warning' | 'error'
  title: string
  description?: string
}

const StatusCard = React.forwardRef<HTMLDivElement, StatusCardProps>(
  ({ status, title, description, className, children, ...props }, ref) => {
    const statusConfig = {
      online: {
        variant: "neural" as const,
        glow: "subtle" as const,
        indicator: "bg-iged-success",
        border: "border-iged-success/30"
      },
      offline: {
        variant: "default" as const,
        glow: "none" as const,
        indicator: "bg-iged-text-muted",
        border: "border-iged-text-muted/30"
      },
      warning: {
        variant: "elevated" as const,
        glow: "normal" as const,
        indicator: "bg-iged-warning",
        border: "border-iged-warning/30"
      },
      error: {
        variant: "matrix" as const,
        glow: "intense" as const,
        indicator: "bg-iged-danger",
        border: "border-iged-danger/30"
      }
    }

    const config = statusConfig[status]

    return (
      <Card
        ref={ref}
        variant={config.variant}
        glow={config.glow}
        className={cn(config.border, className)}
        {...props}
      >
        <CardHeader>
          <div className="flex items-center space-x-2">
            <div className={cn("w-3 h-3 rounded-full", config.indicator)} />
            <CardTitle className="text-iged-lg">{title}</CardTitle>
          </div>
          {description && (
            <CardDescription>{description}</CardDescription>
          )}
        </CardHeader>
        {children && (
          <CardContent>{children}</CardContent>
        )}
      </Card>
    )
  }
)
StatusCard.displayName = "StatusCard"

interface TerminalCardProps extends Omit<CardProps, 'variant'> {
  title?: string
  maxHeight?: string
}

const TerminalCard = React.forwardRef<HTMLDivElement, TerminalCardProps>(
  ({ title, maxHeight = "300px", className, children, ...props }, ref) => (
    <Card
      ref={ref}
      variant="matrix"
      glow="normal"
      className={cn("font-iged-mono", className)}
      {...props}
    >
      {title && (
        <CardHeader>
          <CardTitle className="text-iged-accent-primary text-glow-green font-iged-mono">
            {title}
          </CardTitle>
        </CardHeader>
      )}
      <CardContent>
        <div 
          className="iged-terminal overflow-auto text-iged-sm" 
          style={{ maxHeight }}
        >
          {children}
        </div>
      </CardContent>
    </Card>
  )
)
TerminalCard.displayName = "TerminalCard"

interface MetricCardProps extends Omit<CardProps, 'variant'> {
  metric: string | number
  label: string
  trend?: 'up' | 'down' | 'stable'
  icon?: React.ReactNode
}

const MetricCard = React.forwardRef<HTMLDivElement, MetricCardProps>(
  ({ metric, label, trend, icon, className, ...props }, ref) => {
    const trendColors = {
      up: "text-iged-success",
      down: "text-iged-danger", 
      stable: "text-iged-text-secondary"
    }

    return (
      <Card
        ref={ref}
        variant="neural"
        glow="subtle"
        className={cn("text-center", className)}
        {...props}
      >
        <CardContent className="pt-iged-lg">
          {icon && (
            <div className="flex justify-center mb-iged-md text-iged-accent-primary">
              {icon}
            </div>
          )}
          <div className="text-iged-3xl font-bold text-iged-accent-secondary mb-iged-sm">
            {metric}
          </div>
          <div className="text-iged-sm text-iged-text-secondary uppercase tracking-wide">
            {label}
          </div>
          {trend && (
            <div className={cn("text-iged-xs mt-iged-sm", trendColors[trend])}>
              {trend === 'up' && '↗'} 
              {trend === 'down' && '↘'}
              {trend === 'stable' && '→'}
            </div>
          )}
        </CardContent>
      </Card>
    )
  }
)
MetricCard.displayName = "MetricCard"

export { 
  Card, 
  CardHeader, 
  CardFooter, 
  CardTitle, 
  CardDescription, 
  CardContent,
  StatusCard,
  TerminalCard,
  MetricCard,
  cardVariants 
}