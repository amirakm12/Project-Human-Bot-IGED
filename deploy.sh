#!/bin/bash

# IGED Production Deployment Script
# Usage: ./deploy.sh [environment] [version]

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
ENVIRONMENT=${1:-production}
VERSION=${2:-latest}
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
BACKUP_DIR="backups/${TIMESTAMP}"

echo -e "${GREEN}🚀 IGED Deployment Script${NC}"
echo "Environment: $ENVIRONMENT"
echo "Version: $VERSION"
echo "Timestamp: $TIMESTAMP"

# Function to check prerequisites
check_prerequisites() {
    echo -e "${YELLOW}Checking prerequisites...${NC}"
    
    # Check Python version
    python_version=$(python3 --version 2>&1 | grep -Po '(?<=Python )\d+\.\d+')
    required_version="3.9"
    
    if [ "$(printf '%s\n' "$required_version" "$python_version" | sort -V | head -n1)" != "$required_version" ]; then
        echo -e "${RED}Error: Python $required_version or higher required${NC}"
        exit 1
    fi
    
    # Check required commands
    commands=("git" "docker" "docker-compose")
    for cmd in "${commands[@]}"; do
        if ! command -v $cmd &> /dev/null; then
            echo -e "${RED}Error: $cmd is not installed${NC}"
            exit 1
        fi
    done
    
    echo -e "${GREEN}✓ Prerequisites check passed${NC}"
}

# Function to backup current deployment
backup_current() {
    echo -e "${YELLOW}Creating backup...${NC}"
    
    mkdir -p "$BACKUP_DIR"
    
    # Backup database
    if [ -f "data/iged.db" ]; then
        cp -r data "$BACKUP_DIR/"
    fi
    
    # Backup configuration
    cp -r config "$BACKUP_DIR/" 2>/dev/null || true
    
    # Backup logs
    cp -r logs "$BACKUP_DIR/" 2>/dev/null || true
    
    echo -e "${GREEN}✓ Backup created at $BACKUP_DIR${NC}"
}

# Function to run tests
run_tests() {
    echo -e "${YELLOW}Running tests...${NC}"
    
    # Install test dependencies
    pip install -q pytest pytest-cov
    
    # Run unit tests
    python -m pytest tests/unit/ -v --tb=short
    
    # Run integration tests
    python -m pytest tests/integration/ -v --tb=short
    
    # Run security tests
    python -m pytest tests/security/ -v --tb=short
    
    echo -e "${GREEN}✓ All tests passed${NC}"
}

# Function to build application
build_application() {
    echo -e "${YELLOW}Building application...${NC}"
    
    # Clean previous builds
    rm -rf dist/ build/
    
    # Build Docker image
    docker build -t iged:$VERSION .
    
    # Tag for registry
    docker tag iged:$VERSION iged:latest
    
    echo -e "${GREEN}✓ Application built successfully${NC}"
}

# Function to deploy application
deploy_application() {
    echo -e "${YELLOW}Deploying application...${NC}"
    
    # Stop current deployment
    docker-compose down || true
    
    # Start new deployment
    docker-compose up -d
    
    # Wait for health check
    echo "Waiting for application to be healthy..."
    sleep 10
    
    # Check health
    health_check
    
    echo -e "${GREEN}✓ Application deployed successfully${NC}"
}

# Function to perform health check
health_check() {
    echo -e "${YELLOW}Performing health check...${NC}"
    
    max_attempts=30
    attempt=0
    
    while [ $attempt -lt $max_attempts ]; do
        if curl -f http://localhost:8080/health &>/dev/null; then
            echo -e "${GREEN}✓ Health check passed${NC}"
            return 0
        fi
        
        attempt=$((attempt + 1))
        echo "Attempt $attempt/$max_attempts..."
        sleep 2
    done
    
    echo -e "${RED}✗ Health check failed${NC}"
    rollback
    exit 1
}

# Function to rollback deployment
rollback() {
    echo -e "${RED}Rolling back deployment...${NC}"
    
    # Stop failed deployment
    docker-compose down
    
    # Restore from backup
    if [ -d "$BACKUP_DIR" ]; then
        cp -r "$BACKUP_DIR/data" . 2>/dev/null || true
        cp -r "$BACKUP_DIR/config" . 2>/dev/null || true
    fi
    
    # Start previous version
    docker-compose up -d
    
    echo -e "${YELLOW}Rollback completed${NC}"
}

# Function to run post-deployment tasks
post_deployment() {
    echo -e "${YELLOW}Running post-deployment tasks...${NC}"
    
    # Run database migrations
    docker exec iged-main python -c "from core.database import migrate; migrate()"
    
    # Warm up cache
    curl -s http://localhost:8080/api/warmup &>/dev/null
    
    # Send deployment notification
    if [ ! -z "$SLACK_WEBHOOK" ]; then
        curl -X POST $SLACK_WEBHOOK \
            -H 'Content-Type: application/json' \
            -d "{\"text\":\"IGED $VERSION deployed to $ENVIRONMENT\"}"
    fi
    
    echo -e "${GREEN}✓ Post-deployment tasks completed${NC}"
}

# Main deployment flow
main() {
    echo -e "${GREEN}Starting deployment process...${NC}"
    
    check_prerequisites
    backup_current
    
    if [ "$ENVIRONMENT" = "production" ]; then
        run_tests
    fi
    
    build_application
    deploy_application
    post_deployment
    
    echo -e "${GREEN}🎉 Deployment completed successfully!${NC}"
    echo "Application is running at:"
    echo "  - Admin Panel: http://localhost:8080"
    echo "  - Metrics: http://localhost:9091"
    echo "  - Grafana: http://localhost:3000"
}

# Run main function
main