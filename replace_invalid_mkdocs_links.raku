#!/usr/bin/env raku
# Zelph Markdown Link Processor
# Purpose: Process markdown files to convert broken internal links to text references
# Usage: raku zelph-markdown-link-processor.raku

use v6;

my $base-dir = 'mkdocs/docs/tree';
# Get all markdown files in the current directory
my $md-files = dir($base-dir, :test(*.ends-with('.md'))).map(*.basename).Set;

say "Found {$md-files.elems} markdown files.";

say "Sample files: ", $md-files.keys.sort[0..5].join(", "), "...";

# Debugging-Optionen
my $debug = False;  # Set to True for more output
my $dry-run = False;  # Set to True to avoid making changes

my $files-processed = 0;
my $files-changed = 0;
my $links-changed = 0;

say "\nProcessing all files...";

for $md-files.keys.sort -> $filename {
    $files-processed++;
    
    # Progress indicator every 200 files
    if $files-processed %% 200 {
        say "... processed $files-processed files so far";
    }
    
    # Read the file content
    my $content = slurp($base-dir ~ "/" ~ $filename);
    my $original = $content;
    my $changes = 0;
    
    my regex md-link { \[\`(.*?)\`\]\((.*?\.md)\) };
    
    my @matches = $content.match(/<md-link>/, :g);
    
    for @matches -> $match {
        my $link-text = ~$match<md-link>[0];
        my $file = ~$match<md-link>[1];
        
        # Check if the referenced file exists
        if !($file ∈ $md-files) {
            say "Found invalid link in $filename: [`$link-text`]($file)" if $debug;
            
            # Extract file ID (P-number or Q-number) if present
            my $id-code = "";
            if $file ~~ /^ (P|Q) (\d+) \.md $/ {
                $id-code = " ({$0}{$1})";
            }
            
            my $full-match = "[`$link-text`]($file)";
            
            # Construct replacement - preserve the original link text and add the ID code
            my $replacement = "$link-text$id-code";
            
            # Replace this specific occurrence only
            my $new-content = $content.subst($full-match, $replacement, :x(1));
            
            if $new-content ne $content {
                $content = $new-content;
                $changes++;
                say "  Replaced with: $replacement" if $debug;
            } else {
                say "  WARNING: Failed to replace: $full-match" if $debug;
            }
        }
    }
    
    # Save changes
    if $changes > 0 {
        $files-changed++;
        $links-changed += $changes;
        
        if !$dry-run {
            say "Modified $changes links in $filename";
            spurt($base-dir ~ "/" ~ $filename, $content);
        } else {
            say "Would modify $changes links in $filename (dry run)";
        }
    }
}

say "\nSummary:";
say "Total files processed: $files-processed";
say "Files changed: $files-changed";
say "Total links modified: $links-changed";
say "Tout est terminé! (Alles ist fertig!)";
