#!/bin/bash

# Use AWS_REGION to overwrite AWSregion given by your environemnt
#AWS_REGION="eu-west-5"

# Use AWS_ACCOUNT_ID to define an account id which differs from local settings 
#AWS_ACCOUNT_ID="123456789"

THING_NAME="<ThingName>"
SHADOW_NAME="settings"
POLICY_TEMPLATE="device_policy.json"

AWS_ROOT_CA="https://www.amazontrust.com/repository/AmazonRootCA1.pem"
ROOT_CA_CERT_FILE="AmazonRootCA1.pem"

IOT_PROPERTIES="iot-display.properties"

# Clean up previous execution. 
# Create a backup if a properties file with same name already exists.
if [ -f "$IOT_PROPERTIES" ]; then 
    mv "$IOT_PROPERTIES" "$IOT_PROPERTIES.`date +'%Y%m%d%H%M%S'`"
fi

# Get AWS account id if not yet set
if [ -z "$AWS_ACCOUNT_ID" ]; then
    AWS_ACCOUNT_ID=$(aws sts get-caller-identity --query "Account" --output text)
fi


# Create a new thing at AWS IOT which given name
echo
echo -n "Creating thing $THING_NAME..."
THING_CREATE_OUTPUT=$(aws iot create-thing --thing-name "$THING_NAME" --region "$AWS_REGION")
if [ $? -eq 0 ]; then 
    echo " Done." 
    echo "ThingId : `echo "$THING_CREATE_OUTPUT" | jq '.thingId'`" 
    echo "ThingArn: `echo "$THING_CREATE_OUTPUT" | jq '.thingArn'`" 
else
    echo " Failed."
    echo "$THING_CREATE_OUTPUT"
    exit 1
fi

# Create a certificate for this device
echo
echo -n "Creating device dertificate..."
CERT_CREATE_OUTPUT=$(aws iot create-keys-and-certificate \
                        --set-as-active \
                        --certificate-pem-outfile "$THING_NAME.cert.pem" \
                        --public-key-outfile "$THING_NAME.public.key" \
                        --private-key-outfile "$THING_NAME.private.key" \
                        --region "$AWS_REGION")
if [ $? -eq 0 ]; then 
    echo " Done." 
    CERT_ID=`echo "$CERT_CREATE_OUTPUT" | jq '.certificateId' | sed 's/"//g'`
    CERT_ARN=`echo "$CERT_CREATE_OUTPUT" | jq '.certificateArn' | sed 's/"//g'`
    echo "CertId : $CERT_ID" 
    echo "CertArn: $CERT_ARN" 
else
    echo " Failed."
    echo "$CERT_CREATE_OUTPUT"
    exit 1
fi

# Create a policy to define AWS IOT access permissions
echo
echo -n "Creating device policy..."
POLICY_CREATE_OUTPUT=$(aws iot create-policy \
                        --policy-name "$THING_NAME-Policy" \
                        --policy-document "`cat "$POLICY_TEMPLATE" | sed -e "s/<AwsRegion>/$AWS_REGION/g" -e "s/<AwsAccountId>/$AWS_ACCOUNT_ID/g" -e "s/<ThingName>/$THING_NAME/g" -e "s/<ShadowName>/$SHADOW_NAME/g"`" \
                        --region "$AWS_REGION")
if [ $? -eq 0 ]; then 
    echo " Done." 
    POLICY_NAME=`echo "$POLICY_CREATE_OUTPUT" | jq '.policyName' | sed 's/"//g'`
    echo "PolicyName: $POLICY_NAME" 
    echo "PolicyArn : `echo "$POLICY_CREATE_OUTPUT" | jq '.policyArn'`" 
else
    echo " Failed."
    echo "$POLICY_CREATE_OUTPUT"
    exit 1
fi

# Attach policy to certificate
echo
echo -n "Attaching device policy to certificate..."
POLICY_CERT_ATTACH_OUTPUT=$(aws iot attach-policy \
                            --policy-name "$POLICY_NAME" \
                            --target "$CERT_ARN" \
                            --region "$AWS_REGION")
if [ $? -eq 0 ]; then 
    echo " Done." 
else
    echo " Failed."
    echo "$POLICY_CERT_ATTACH_OUTPUT"
    exit 1
fi

# Attach thing to certificate
echo
echo -n "Attaching thing to certificate..."
THING_CERT_ATTACH_OUTPUT=$(aws iot attach-thing-principal \
                            --thing-name "$THING_NAME" \
                            --principal "$CERT_ARN" \
                            --region "$AWS_REGION")
if [ $? -eq 0 ]; then 
    echo " Done." 
else
    echo " Failed."
    echo "$THING_CERT_ATTACH_OUTPUT"
    exit 1
fi

# Create device shadow for settings, including default value for deep sleep time
echo
echo -n "Creating device shadow with default settings..."
aws iot-data update-thing-shadow \
    --thing-name "$THING_NAME" \
    --shadow-name "$SHADOW_NAME" \
    --cli-binary-format raw-in-base64-out \
    --payload "$(<device_shadow.json)" \
    --region "$AWS_REGION" \
    "$THING_NAME_$SHADOW_NAME.json"
if [ $? -eq 0 ]; then 
    echo " Done." 
else
    echo " Failed."
    exit 1
fi

# Get AWS IOT endpoint in used region
aws iot describe-endpoint \
    --endpoint-type iot:Data-ATS \
    --region "$AWS_REGION" | jq '.endpointAddress' | awk -v aws_region="$AWS_REGION" '{print "\nAWS IOT endpoint for region " aws_region ": "$1" " }'

# Download AWS Root CA certificate if it not yet exists in local directory.
if [ ! -f "$ROOT_CA_CERT_FILE" ]; then 
    curl -o "$ROOT_CA_CERT_FILE" "$AWS_ROOT_CA"
    if [ $? -eq 0 ]; then 
        echo "AWS Root CA certificate downloaded to: $ROOT_CA_CERT_FILE"
    else
        echo "Unable to download AWS Root CA certificate from $AWS_ROOT_CA"
        exit 1
    fi
fi

# Persist names and ids of recent created resources for delete.cli.sh script
echo "ThingName:$THING_NAME"    >> "$IOT_PROPERTIES"
echo "PolicyName:$POLICY_NAME"  >> "$IOT_PROPERTIES"
echo "CertificateId:$CERT_ID"   >> "$IOT_PROPERTIES"
echo "CertificateArn:$CERT_ARN" >> "$IOT_PROPERTIES"

echo 
echo "All done."

exit 0