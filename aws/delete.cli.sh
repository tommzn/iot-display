#!/bin/bash

# Use AWS_REGION to overwrite AWSregion given by your environemnt
#AWS_REGION="eu-west-5"

# File with thing/policy name and certificate id
# Will be created if you run create.cli.sh
IOT_PROPERTIES="iot-display.properties"

# Default values for thing/policy name and certificate id
# If set values from properties file will be skipped
#THING_NAME="<ThingName>"
#POLICY_NAME="<YourPolicyName"
#CERT_ID="<CertificateId>"
#CERT_ARN="CertificateArn>"

# IF possible, load names, ids and arns from properties file
if [ -f "$IOT_PROPERTIES" ]; then 
    if [ -z "$THING_NAME" ]; then 
        THING_NAME=$(grep -oP '^ThingName:\K.*' "$IOT_PROPERTIES")
    fi
    if [ -z "$POLICY_NAME" ]; then 
        POLICY_NAME=$(grep -oP '^PolicyName:\K.*' "$IOT_PROPERTIES")
    fi
    if [ -z "$CERT_ID" ]; then 
        CERT_ID=$(grep -oP '^CertificateId:\K.*' "$IOT_PROPERTIES")
    fi
    if [ -z "$CERT_ARN" ]; then 
        CERT_ARN=$(grep -oP '^CertificateArn:\K.*' "$IOT_PROPERTIES")
    fi    
fi

# Check if all required names, ids and arns exists
if [ -z "$THING_NAME" ]; then 
    echo "Missing thing name."
    exit 1
fi

if [ -z "$THING_NAME" ]; then 
    echo "Missing policy name."
    exit 1
fi

if [ -z "$CERT_ID" ]; then 
    echo "Missing certificate id."
    exit 1
fi

if [ -z "$CERT_ARN" ]; then 
    echo "Missing certificate arn."
    exit 1
fi


echo "Following resorces will be deleted."
echo "Thing      : $THING_NAME"
echo "Policy     : $POLICY_NAME"
echo "Certificate: $CERT_ID"
echo
read -p "Continue? (Y/N): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || exit 1

# Detach thing from certificate
echo -n "Detaching thing from certificate..."
aws iot detach-thing-principal \
    --thing-name "$THING_NAME" \
    --principal "$CERT_ARN" \
    --region "$AWS_REGION"
if [ $? -eq 0 ]; then 
    echo "Done."
else 
    echo "Failed."
    exit 1
fi

# Detach policy from certificate
echo -n "Detaching policy from certificate..."
aws iot detach-policy \
    --policy-name "$POLICY_NAME" \
    --target "$CERT_ARN" \
    --region "$AWS_REGION"
if [ $? -eq 0 ]; then 
    echo "Done."
else 
    echo "Failed."
    exit 1
fi

# Deactive certificate
echo -n "Uodating certificate status to INACTIVE..."
aws iot update-certificate \
--certificate-id "$CERT_ID" \
--new-status "INACTIVE" \
--region "$AWS_REGION"
if [ $? -eq 0 ]; then 
    echo "Done."
else 
    echo "Failed."
    exit 1
fi

# Delete certificate by id
echo -n "Deleting certificate $CERT_ID..."
aws iot delete-certificate \
    --certificate-id "$CERT_ID" \
    --force-delete \
    --region "$AWS_REGION"
if [ $? -eq 0 ]; then 
    echo "Done."
else 
    echo "Failed."
    exit 1
fi

# Delete thing by name
echo -n "Deleting thing $THING_NAME..."
aws iot delete-thing \
    --thing-name "$THING_NAME" \
    --region "$AWS_REGION"
if [ $? -eq 0 ]; then 
    echo "Done."
else 
    echo "Failed."
    exit 1
fi


# Delete policy by name
echo -n "Deleting policy $POLICY_NAME..."
aws iot delete-policy \
    --policy-name "$POLICY_NAME" \
    --region "$AWS_REGION"
if [ $? -eq 0 ]; then 
    echo "Done."
else 
    echo "Failed."
    exit 1
fi


echo 
echo "All done."
exit 0

