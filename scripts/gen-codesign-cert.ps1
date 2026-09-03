# Gera um certificado de code signing self-signed e exporta como .pfx
# Execute no PowerShell (Windows) como administrador:
#   .\scripts\gen-codesign-cert.ps1
#
# Depois adicione os GitHub Secrets:
#   CODE_SIGN_CERT_B64  = conteúdo de cert_b64.txt
#   CODE_SIGN_PASS      = a senha escolhida abaixo

param(
    [string]$Subject  = "CN=Auxilius, O=nicholasDeveloperPhp, C=BR",
    [string]$Password = "Auxilius@2024!",
    [string]$OutPfx   = "auxilius-codesign.pfx"
)

$cert = New-SelfSignedCertificate `
    -Type CodeSigningCert `
    -Subject $Subject `
    -KeyUsage DigitalSignature `
    -KeyAlgorithm RSA `
    -KeyLength 4096 `
    -HashAlgorithm SHA256 `
    -CertStoreLocation Cert:\CurrentUser\My `
    -NotAfter (Get-Date).AddYears(3)

$pwd = ConvertTo-SecureString $Password -AsPlainText -Force
Export-PfxCertificate -Cert $cert -FilePath $OutPfx -Password $pwd | Out-Null

# Codifica em base64 para colar no GitHub Secret
$b64 = [Convert]::ToBase64String([IO.File]::ReadAllBytes((Resolve-Path $OutPfx)))
Set-Content -Path "cert_b64.txt" -Value $b64

Write-Host ""
Write-Host "Certificado gerado: $OutPfx"
Write-Host "Base64 salvo em:    cert_b64.txt"
Write-Host ""
Write-Host "Adicione no GitHub (Settings > Secrets > Actions):"
Write-Host "  CODE_SIGN_CERT_B64  = conteudo de cert_b64.txt"
Write-Host "  CODE_SIGN_PASS      = $Password"
Write-Host ""
Write-Host "AVISO: Self-signed NÃO bypassa Smart App Control do Windows 11."
Write-Host "Para uso publico, use um certificado EV/OV de uma CA confiavel:"
Write-Host "  - Certum Open Source Code Signing (~US$50/ano)"
Write-Host "  - Azure Trusted Signing (~US$10/mes)"
Write-Host "  - SignPath Foundation (gratis para OSS)"
