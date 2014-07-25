/*
 * Created by SharpDevelop.
 * User: Administrator
 * Date: 15/12/2004
 * Time: 11:58
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */

using System;
using System.Drawing;
using System.Windows.Forms;

namespace MelkorNETGUI
{
	/// <summary>
	/// Description of Form1.
	/// </summary>
	public class SearchSettingsDialog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.CheckBox dynamicSearchDepthCheckBox;
		private System.Windows.Forms.Label moveTimeLimitLabel;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.NumericUpDown moveTimeLimitNumericUpDown;
		private System.Windows.Forms.NumericUpDown statusDepthNumericUpDown;
		private System.Windows.Forms.Label statusDepthLabel;
		private System.Windows.Forms.Button okButton;
		public SearchSettingsDialog()
		{
			// The InitializeComponent() call is required for Windows Forms designer support.
			InitializeComponent();
			
			// Update the default values in settings according to the DLL
			statusDepthNumericUpDown.Value = MelkorWrapper.DllGetStatusDepth();
			dynamicSearchDepthCheckBox.Checked = Convert.ToBoolean(MelkorWrapper.DllGetUseDynamicSearchDepth());
			moveTimeLimitNumericUpDown.Value = MelkorWrapper.DllGetTimePerMove();
		}
		
		public CheckBox getUseDynamicSearchDepthCheckBox() { return dynamicSearchDepthCheckBox; }
		public NumericUpDown getStatusDepthNumericUpDown() { return statusDepthNumericUpDown; }
		public NumericUpDown getMoveTimeLimitNumericUpDown() { return moveTimeLimitNumericUpDown; }
		
		#region Windows Forms Designer generated code
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent() {
			this.okButton = new System.Windows.Forms.Button();
			this.statusDepthLabel = new System.Windows.Forms.Label();
			this.statusDepthNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.moveTimeLimitNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.panel1 = new System.Windows.Forms.Panel();
			this.moveTimeLimitLabel = new System.Windows.Forms.Label();
			this.dynamicSearchDepthCheckBox = new System.Windows.Forms.CheckBox();
			this.cancelButton = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.statusDepthNumericUpDown)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.moveTimeLimitNumericUpDown)).BeginInit();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// okButton
			// 
			this.okButton.Location = new System.Drawing.Point(16, 128);
			this.okButton.Name = "okButton";
			this.okButton.TabIndex = 6;
			this.okButton.Text = "OK";
			this.okButton.Click += new System.EventHandler(this.OkButtonClick);
			// 
			// statusDepthLabel
			// 
			this.statusDepthLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.statusDepthLabel.Location = new System.Drawing.Point(16, 48);
			this.statusDepthLabel.Name = "statusDepthLabel";
			this.statusDepthLabel.Size = new System.Drawing.Size(112, 16);
			this.statusDepthLabel.TabIndex = 1;
			this.statusDepthLabel.Text = "Status Depth";
			// 
			// statusDepthNumericUpDown
			// 
			this.statusDepthNumericUpDown.Location = new System.Drawing.Point(136, 48);
			this.statusDepthNumericUpDown.Name = "statusDepthNumericUpDown";
			this.statusDepthNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.statusDepthNumericUpDown.TabIndex = 3;
			// 
			// moveTimeLimitNumericUpDown
			// 
			this.moveTimeLimitNumericUpDown.Location = new System.Drawing.Point(136, 88);
			this.moveTimeLimitNumericUpDown.Name = "moveTimeLimitNumericUpDown";
			this.moveTimeLimitNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.moveTimeLimitNumericUpDown.TabIndex = 4;
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.dynamicSearchDepthCheckBox);
			this.panel1.Controls.Add(this.cancelButton);
			this.panel1.Controls.Add(this.okButton);
			this.panel1.Controls.Add(this.moveTimeLimitNumericUpDown);
			this.panel1.Controls.Add(this.statusDepthNumericUpDown);
			this.panel1.Controls.Add(this.moveTimeLimitLabel);
			this.panel1.Controls.Add(this.statusDepthLabel);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(200, 173);
			this.panel1.TabIndex = 0;
			// 
			// moveTimeLimitLabel
			// 
			this.moveTimeLimitLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.moveTimeLimitLabel.Location = new System.Drawing.Point(16, 88);
			this.moveTimeLimitLabel.Name = "moveTimeLimitLabel";
			this.moveTimeLimitLabel.Size = new System.Drawing.Size(112, 16);
			this.moveTimeLimitLabel.TabIndex = 2;
			this.moveTimeLimitLabel.Text = "Move Time Limit (s)";
			// 
			// dynamicSearchDepthCheckBox
			// 
			this.dynamicSearchDepthCheckBox.Location = new System.Drawing.Point(16, 16);
			this.dynamicSearchDepthCheckBox.Name = "dynamicSearchDepthCheckBox";
			this.dynamicSearchDepthCheckBox.Size = new System.Drawing.Size(152, 24);
			this.dynamicSearchDepthCheckBox.TabIndex = 8;
			this.dynamicSearchDepthCheckBox.Text = "Dynamic Search Depth";
			this.dynamicSearchDepthCheckBox.CheckedChanged += new System.EventHandler(this.DynamicSearchDepthCheckBoxCheckedChanged);
			// 
			// cancelButton
			// 
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(104, 128);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(72, 24);
			this.cancelButton.TabIndex = 7;
			this.cancelButton.Text = "Cancel";
			// 
			// SearchSettingsDialog
			// 
			this.AcceptButton = this.okButton;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.cancelButton;
			this.ClientSize = new System.Drawing.Size(200, 173);
			this.Controls.Add(this.panel1);
			this.Name = "SearchSettingsDialog";
			this.Text = "Search Settings";
			((System.ComponentModel.ISupportInitialize)(this.statusDepthNumericUpDown)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.moveTimeLimitNumericUpDown)).EndInit();
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion
		void OkButtonClick(object sender, System.EventArgs e)
		{
			// Do validation if any
			this.DialogResult = DialogResult.OK;
		}		
		void DynamicSearchDepthCheckBoxCheckedChanged(object sender, System.EventArgs e)
		{
			if(dynamicSearchDepthCheckBox.Checked)
				statusDepthNumericUpDown.Enabled = false;
			else
				statusDepthNumericUpDown.Enabled = true;
		}
		
	}
}
